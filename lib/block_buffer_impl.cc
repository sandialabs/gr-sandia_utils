/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "block_buffer_impl.h"
#include "gnuradio/sandia_utils/constants.h"
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <gnuradio/io_signature.h>

namespace gr {
namespace sandia_utils {

block_buffer::sptr
block_buffer::make(size_t itemsize, uint64_t nsamples, float samp_rate, bool pass_data)
{
    return gnuradio::get_initial_sptr(
        new block_buffer_impl(itemsize, nsamples, samp_rate, pass_data));
}

/*
 * The private constructor
 */
block_buffer_impl::block_buffer_impl(size_t itemsize,
                                     uint64_t nsamples,
                                     float samp_rate,
                                     bool pass_data)
    : gr::block("block_buffer",
                gr::io_signature::make(1, 1, itemsize),
                gr::io_signature::make(1, 1, itemsize)),
      d_itemsize(itemsize),
      d_samp_rate(samp_rate),
      d_pass_data(pass_data)
{
    init_buffers(nsamples);

    // small spinlock timeout at beginning till rate is reading
    d_usleep = 10;

    d_last_abs_read_idx = 0;
    d_current_rx_time_tag.offset = 0;
    d_current_rx_time_tag.key = PMTCONSTSTR__rx_time();
    d_current_rx_time_tag.value =
        pmt::make_tuple(pmt::from_uint64(0), pmt::from_double(0));
    d_rx_time_tags.push(d_current_rx_time_tag);

    d_init = false;

    // don't propagate tags (we handle them manually)
    set_tag_propagation_policy(gr::block::TPP_DONT);
}

/*
 * Our virtual destructor.
 */
block_buffer_impl::~block_buffer_impl()
{
    for (int i = 0; i < 3; i++) {
        if (d_buf[i].ptr != nullptr) {
            free(d_buf[i].ptr);
            d_buf[i].ptr = nullptr;
        }
    }
}

void block_buffer_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    // This block is more concerned about processing inputs than producing
    // outputs. If there is input available, then we want work to be called.

    // If d_latest_valid is true, then we have samples ready to write out,
    // so we require no (0) inputs. If d_latest_valid is false, we still
    // want to process inputs, regardless of whether we end up writing
    // anything, so we need at least 1 input.
    ninput_items_required[0] = d_latest_valid ? 0 : 1;
}

void block_buffer_impl::set_nsamples(uint64_t nsamples)
{
    std::lock_guard<std::mutex> lock(work_mutex);

    d_next_nsamples = nsamples;
    d_read_idx = 0;
}

void block_buffer_impl::init_buffers(uint64_t nsamples)
{

    d_nsamples = nsamples;
    for (int i = 0; i < 3; i++) {
        if (d_buf[i].ptr != nullptr) {
            free(d_buf[i].ptr);
            d_buf[i].ptr = nullptr;
        }
        d_buf[i].ptr = (void*)malloc(d_nsamples * d_itemsize);
    }

    d_write_idx = d_read_idx = 0;

    d_reading = 0;
    d_writing = 1;
    d_latest = -1;
    d_latest_valid = false;

    // reset flag
    d_next_nsamples = 0;
}

int block_buffer_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    if (!d_init) {
        d_nreserved = 0.05 * max_noutput_items();
        d_input_buff_size = detail()->input(0)->max_possible_items_available();
        d_init = true;
    }

    // indices to the current input/output buffers
    int in_idx = 0;
    int out_idx = 0;

    std::lock_guard<std::mutex> lock(work_mutex);

    // do not pass data if not told to do so, and avoid spinlock
    // when we're actually consuming data quicker than it is being produced
    if ((not d_pass_data) or (ninput_items[0] == 0)) {
        consume_each(ninput_items[0]);
        usleep(d_usleep);
        return 0;
    }

    // read
    while (in_idx < ninput_items[0]) {

        // if at the beginning of a buffer, record the absolute index
        if (d_read_idx == 0) {
            // reset if necessary
            if (d_next_nsamples) {
                init_buffers(d_next_nsamples);
            }

            d_buf[d_reading].abs_read_idx = nitems_read(0) + in_idx;
            d_buf[d_reading].tags.clear();
            d_buf[d_reading].rx_time = pmt::PMT_NIL;
        }

        // read as much as we can into the current buffer
        size_t to_read =
            std::min<size_t>((size_t)(ninput_items[0] - in_idx), d_nsamples - d_read_idx);


        // update current rate
        std::vector<tag_t> rate_tags;
        get_tags_in_window(
            rate_tags, 0, in_idx, in_idx + to_read, PMTCONSTSTR__rx_rate());
        if (rate_tags.size() > 0) {
            // use first tag only?
            double rate = pmt::to_double(rate_tags[0].value);

            // set sleep to be 2/4 of the input buffer size
            d_usleep = (int)((double)d_input_buff_size / 2.0 / rate * 1e6);
        }

        // find overflow tags
        std::vector<tag_t> overflow_tags;
        get_tags_in_window(
            overflow_tags, 0, in_idx, in_idx + to_read, PMTCONSTSTR__overflow());

        // check for bad tags in samples we're about to read
        std::vector<tag_t> tags;
        get_tags_in_window(tags,
                           0,
                           in_idx,
                           in_idx + to_read,
                           PMTCONSTSTR__rx_time()); // rx_time tag is sent on overflow,
                                                    // rate change, or frequency change
        if ((tags.size() > 0) or (overflow_tags.size() > 0)) {

            // keep track of all received rx_time tags for timing purposes
            for (const tag_t& tag : tags) {
                // if we just restarted, this might be the tag we just added
                if (!(d_rx_time_tags.back() == tag)) {
                    d_rx_time_tags.push(tag);
                }
            }

            // if there are more rx_time tags than overflow tags, we need to reset because
            // we don't know if they are overflows (like usrp tags)
            bool overflows = false;
            if (tags.size() > overflow_tags.size()) {
                overflows = true;
            }
            // if there are more overflow tags than rx_time tags, then something weird has
            // happened
            else if (tags.size() < overflow_tags.size()) {
                // what to do here? for now, just reset
                overflows = true;
            }
            // this if should be unecessary - they are definitely equal in size if we
            // reach this point
            else if (tags.size() == overflow_tags.size()) {
                for (size_t i = 0; i < tags.size(); i++) {
                    // if the tag offsets don't match, we have a problem and need to reset
                    // (are these ordered?)
                    if (tags[i].offset != overflow_tags[i].offset) {
                        overflows = true;
                    }
                    // if any of the overflow tags are marked true, we have an overflow
                    if (pmt::to_bool(overflow_tags[i].value)) {
                        overflows = true;
                    }
                }
            }

            // if the tag is at the beginning of the buffer, we can assume that we
            // are starting to store from that point forward
            if ((tags.size() == 1) and
                (tags[0].offset == d_buf[d_reading].abs_read_idx)) {
                d_buf[d_reading].rx_time = tags[0].value;
            } else if (overflows) {
                // reset to pont of last tag
                d_read_idx = 0;
                in_idx = tags.back().offset - nitems_read(0);
                GR_LOG_DEBUG(d_logger,
                             boost::format("bad tags... starting buffer %d over at %d") %
                                 d_reading % in_idx);
                GR_LOG_DEBUG(d_logger,
                             boost::format("the size of overflow_tags is %ld") %
                                 overflow_tags.size());
                if (overflow_tags.size()) {
                    GR_LOG_DEBUG(d_logger,
                                 boost::format("the overflow tags value is %d") %
                                     pmt::to_bool(overflow_tags[0].value));
                }
                GR_LOG_DEBUG(d_logger,
                             boost::format("the size of tags is %ld") % tags.size());
                GR_LOG_DEBUG(d_logger,
                             boost::format("the abs_read index is %ld") %
                                 d_buf[d_reading].abs_read_idx);
                if (tags.size()) {
                    GR_LOG_DEBUG(d_logger,
                                 boost::format("the tags[0] offset is %ld") %
                                     tags[0].offset);
                }
                continue;
            }
        }

        // copy into buffer
        char* dst = static_cast<char*>(d_buf[d_reading].ptr) + d_itemsize * d_read_idx;
        const char* src = static_cast<const char*>(input_items[0]) + d_itemsize * in_idx;
        memcpy(dst, src, d_itemsize * to_read);

        // also copy tags
        get_tags_in_window(tags, 0, in_idx, in_idx + to_read);
        d_buf[d_reading].tags.insert(
            d_buf[d_reading].tags.end(), tags.begin(), tags.end());

        d_read_idx += to_read;
        in_idx += to_read;

        // check if the buffer is full
        if (d_read_idx == d_nsamples) {
            // buffer is full, switch to new buffer
            d_read_idx = 0;
            d_latest = d_reading;
            d_reading = 3 - d_writing - d_latest;
            if (d_latest_valid == false) {
                // jump start the writing buffer if it has been waiting
                d_write_idx = 0;
                d_writing = d_latest;
            }
            d_latest_valid = true;
        }
    }

    consume_each(ninput_items[0]);

    // write
    while (d_latest_valid && out_idx < noutput_items - d_nreserved) {

        // if at the beginning of a buffer, copy all tags onto the output stream
        if (d_write_idx == 0) {
            d_buf[d_writing].abs_write_idx = nitems_written(0) + out_idx;

            uint64_t offset =
                d_buf[d_writing].abs_read_idx - d_buf[d_writing].abs_write_idx;
            for (tag_t& tag : d_buf[d_writing].tags) {
                tag.offset -= offset;
                add_item_tag(0, tag);
            }

            // pop from the d_rx_time_tags queue until d_current_rx_time_tag has the last
            // rx_time tag received before the beginning of this buffer
            while (!d_rx_time_tags.empty() &&
                   d_buf[d_writing].abs_read_idx >= d_rx_time_tags.front().offset) {
                d_current_rx_time_tag = d_rx_time_tags.front();
                d_rx_time_tags.pop();
            }

            // number of samples skipped since the last buffer written
            int64_t numsamples_skipped =
                (d_buf[d_writing].abs_read_idx == 0)
                    ? 0
                    : std::max<unsigned long long>(d_buf[d_writing].abs_read_idx -
				   d_last_abs_read_idx - d_nsamples,
                               0lu);

            // if there's not already an rx_time tag, and some samples were skipped,
            // estimate and add an rx_time tag
            if (pmt::eqv(d_buf[d_writing].rx_time, pmt::PMT_NIL) &&
                numsamples_skipped > 0) {
                // estimate the time elapsed since last received rx_time tag
                double time_elapsed =
                    (d_buf[d_writing].abs_read_idx - d_current_rx_time_tag.offset) /
                    d_samp_rate;
                // add that time to the last received rx_time
                double frac_seconds =
                    pmt::to_double(pmt::tuple_ref(d_current_rx_time_tag.value, 1)) +
                    time_elapsed;
                // subtract any whole seconds
                uint64_t seconds = floor(frac_seconds);
                frac_seconds -= seconds;
                // add on the whole seconds from the last rx_time
                seconds += pmt::to_uint64(pmt::tuple_ref(d_current_rx_time_tag.value, 0));
                // create the rx_time tag and add it to the stream
                d_buf[d_writing].rx_time = pmt::make_tuple(
                    pmt::from_uint64(seconds), pmt::from_double(frac_seconds));
                add_item_tag(0,
                             d_buf[d_writing].abs_write_idx,
                             PMTCONSTSTR__rx_time(),
                             d_buf[d_writing].rx_time);
            }

            // also insert a start of block tag
            add_item_tag(0,
                         d_buf[d_writing].abs_write_idx,
                         PMTCONSTSTR__BLOCK(),
                         pmt::mp((uint64_t)numsamples_skipped));
            d_last_abs_read_idx = d_buf[d_writing].abs_read_idx;
        }

        // write as much as we can from the current buffer
        size_t to_write = std::min<size_t>((size_t)(noutput_items - d_nreserved - out_idx),
                                   d_nsamples - d_write_idx);

        // write from buffer
        char* dst = static_cast<char*>(output_items[0]) + d_itemsize * out_idx;
        char* src = static_cast<char*>(d_buf[d_writing].ptr) + d_itemsize * d_write_idx;
        memcpy(dst, src, d_itemsize * to_write);

        d_write_idx += to_write;
        out_idx += to_write;

        // check if the buffer is empty
        if (d_write_idx == d_nsamples) {
            // buffer is empty, switch to new buffer
            if (d_latest == d_writing) {
                // no new buffer available
                d_latest_valid = false;

            } else {
                // switch to latest filled buffer
                d_write_idx = 0;
                d_writing = d_latest;
            }
        }
    }

    // Tell runtime system how many output items we produced.
    return out_idx;
}
} /* namespace sandia_utils */
} /* namespace gr */
