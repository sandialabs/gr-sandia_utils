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

#include "file_source_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/filesystem.hpp>
#include <fstream>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Create a file source.
 *
 * Opens \p filename as a source of items into a flowgraph. The
 * data is expected to be in the format specified by \p type. The
 * \p itemsize of the block determines the conversion from bits
 * to items.
 *
 * If \p repeat is turned on, the file will repeat the file after
 * it's reached the end.
 *
 * \param itemsize  the size of each item in the file, in bytes
 * \param filename  name of the file to source from
 * \param type file type, Example Values = message, raw, raw_header, bluefile
 * \param repeat  repeat file from start
 * \param force_new Force open new file upon command, regardless of current status
 */
file_source::sptr file_source::make(
    size_t itemsize, const char* filename, const char* type, bool repeat, bool force_new)
{
    return gnuradio::get_initial_sptr(
        new file_source_impl(itemsize, filename, type, repeat, force_new));
}

/**
 * Constructor
 *
 * @param itemsize - per item size in bytes
 * @param filename - filename to open as source.
 * @param type - type of file input, Example Values = message, raw, raw_header, bluefile
 * @param repeat - repeat a single file over and over.
 * @param force_new - Force open new file upon command, regardless of current status
 */
file_source_impl::file_source_impl(
    size_t itemsize, const char* filename, const char* type, bool repeat, bool force_new)
    : gr::sync_block("file_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(strcmp(type, "message") == 0 ? 0 : 1,
                                            strcmp(type, "message") == 0 ? 0 : 1,
                                            itemsize)),
      d_itemsize(itemsize),
      d_repeat(repeat),
      d_force_new(force_new),
      d_tag_on_open(false),
      d_repeat_cnt(0),
      d_add_begin_tag(pmt::PMT_NIL),
      d_tag_now(false),
      d_first_pass(true),
      d_file_queue_depth(DEFAULT_FILE_QUEUE_DEPTH),
      d_method_count(0),
      d_msg_hop_period(0)
{
    d_output_type = std::string(type);
    d_filename = std::string(filename);

    // get reader
    if (strcmp(type, "message") == 0) {
        // register output port
        message_port_register_out(OUT_KEY);
    } else {
        if (strcmp(type, "raw") == 0) {
            d_reader = file_reader_base::sptr(new file_reader_base(itemsize, d_logger));
        } else if (strcmp(type, "raw_header") == 0) {
            d_reader =
                file_reader_base::sptr(new file_reader_raw_header(itemsize, d_logger));
        }
#ifdef HAVE_BLUEFILE_LIB
        else if (strcmp(type, "bluefile") == 0) {
            d_reader =
                file_reader_base::sptr(new file_reader_bluefile(itemsize, d_logger));
        }
#endif
        else {
            throw std::runtime_error(
                str(boost::format("Invalid file source format %s") % type));
        }

        // empty list of tags for now
        d_tags.resize(0);

        // open file if specified
        if (filename[0] != '\0') {
            GR_LOG_DEBUG(d_logger, str(boost::format("Opening IQ file: %s") % filename));
            this->open(filename, d_repeat);
        }
    }

    // register message ports
    message_port_register_in(pmt::intern("pdu"));
    set_msg_handler(pmt::intern("pdu"),
                    boost::bind(&file_source_impl::handle_msg, this, _1));
}

/*
 * Our virtual destructor.
 */
file_source_impl::~file_source_impl()
{ /* NOOP */
}

bool file_source_impl::start()
{
    // start message generation thread
    if (d_output_type == "message") {
        // NOTE: d_finished should be something explicitly thread safe. But since
        // nothing breaks on concurrent access, I'll just leave it as bool.
        d_finished = false;
        d_thread = std::shared_ptr<gr::thread::thread>(
            new gr::thread::thread(boost::bind(&file_source_impl::run, this)));
    }

    return block::start();
}

bool file_source_impl::stop()
{
    if (d_output_type == "message") {
        // Shut down the thread
        d_finished = true;
        d_thread->interrupt();
        d_thread->join();
    }

    return block::stop();
}

void file_source_impl::run()
{
    // open file
    std::string current_file = d_filename;
    std::ifstream file;
    GR_LOG_DEBUG(d_logger, str(boost::format("Opening message file: %s") % current_file));
    file.open(current_file, std::ios::in | std::ios::binary);
    if (not file.is_open()) {
        throw std::runtime_error(
            str(boost::format("Unable to open file %s") % current_file));
    }

    while (!d_finished) {
        boost::this_thread::sleep(
            boost::posix_time::milliseconds(static_cast<long>(d_msg_hop_period)));
        if (d_finished) {
            // close file
            file.close();

            return;
        }

        // read in message length
        uint32_t len;
        file.read((char*)&len, sizeof(uint32_t));
        if (file.fail()) {
            GR_LOG_ERROR(d_logger, str(boost::format("Unable to read message length")));
            break;
        }
        GR_LOG_DEBUG(d_logger, str(boost::format("Message length: %u") % len));

        // read in message
        if (len != 0) {
            std::string st((size_t)len, 0);
            file.read((char*)st.c_str(), len);
            if (file.fail()) {
                GR_LOG_ERROR(d_logger, str(boost::format("Unable to read message")));
                break;
            }

            // deserialize str
            pmt::pmt_t msg;
            try {
                msg = pmt::deserialize_str(st);
            } catch (...) {
                GR_LOG_ERROR(d_logger,
                             str(boost::format("Unable to deserialize message")));
                break;
            }
            message_port_pub(OUT_KEY, msg);

            // check if end of file and whether to reopen
            if (file.peek() == EOF) {
                if (d_repeat) {
                    file.seekg(0, file.beg);
                } else {
                    break;
                }
            }
        } // end len != 0;
    }     // end while (!d_finished)
} // end run()

// set message hop period
void file_source_impl::set_msg_hop_period(int period_ms)
{
    gr::thread::scoped_lock lock(fp_mutex);
    if (period_ms > 0) {
        d_msg_hop_period = period_ms;
    }
}

bool file_source_impl::seek(long seek_point, int whence)
{
    return d_reader->seek(seek_point, whence);
}


void file_source_impl::open(const char* filename, bool repeat)
{
    gr::thread::scoped_lock lock(d_setlock);

    // update repeat status
    d_repeat = repeat;

    // only open if we are forcing open.  otherwise, place file onto
    // queue and let work function handle Opening
    if (d_force_new) {
        GR_LOG_DEBUG(d_logger, "Forcing file close and new file open");
        d_reader->close();
        d_reader->open(filename);
        d_tag_now = true;
    } else {
        if (d_file_queue.size() >= d_file_queue_depth) {
            // modifynig queue so protect
            gr::thread::scoped_lock lock(fp_mutex);

            // clear queue
            GR_LOG_DEBUG(d_logger, "Maximum number of file entries reached...resetting");
            while (d_file_queue.size()) {
                d_file_queue.pop();
            }

            // add new file and tag
            d_file_queue.push(std::make_pair(std::string(filename), true));
        } else {
            // add new file and tag if necessary
            bool add_tags = d_first_pass or d_tag_now or d_tag_on_open;
            d_file_queue.push(std::make_pair(std::string(filename), add_tags));
            d_first_pass = false;
        }
    }

    // attempt to open next files
    open_next();
}

void file_source_impl::open_next()
{
    if ((not d_reader->is_open()) and (d_file_queue.size())) {
        // obtain exclusive access for duration of this scope
        gr::thread::scoped_lock lock(fp_mutex);

        std::pair<std::string, bool> value = d_file_queue.front();
        d_reader->open(value.first.c_str());
        d_tag_now = value.second;

        // remove file
        d_file_queue.pop();
    }
}

void file_source_impl::close()
{
    if (d_reader->is_open()) {
        d_reader->close();
    }
}

void file_source_impl::set_begin_tag(pmt::pmt_t tag)
{
    d_add_begin_tag = tag;
    // d_tag_now = not pmt::equal(d_add_begin_tag, pmt::PMT_NIL);
}

void file_source_impl::handle_msg(pmt::pmt_t pdu)
{
    // is_pair() will pass both dictionaries and pairs (possible PDUs...)
    if (!pmt::is_pair(pdu)) {
        return;
    }

    try {
        // Will fail if msg is not a dictionary or PDU (dict/pmt pair)
        pmt::pmt_t x = pmt::dict_keys(pdu);
    } catch (const pmt::wrong_type& e) {
        return;
    }

    // we have a dictionary or a pair of which one element is a dict
    pmt::pmt_t fname_pmt;
    fname_pmt = pmt::dict_ref(pdu, FNAME_KEY, pmt::PMT_NIL);

    if (not pmt::equal(fname_pmt, pmt::PMT_NIL)) {
        // to be safe, lets check if file even exists
        std::string fname = pmt::symbol_to_string(fname_pmt);
        if (boost::filesystem::exists(fname)) {
            this->open(fname.c_str(), d_repeat);
        }
    } else {
        // could still be a PDU
        pmt::pmt_t car = pmt::car(pdu);
        fname_pmt = pmt::dict_ref(car, FNAME_KEY, pmt::PMT_NIL);
        if (not pmt::equal(fname_pmt, pmt::PMT_NIL)) {
            // to be safe, lets check if file even exists
            std::string fname = pmt::symbol_to_string(fname_pmt);
            if (boost::filesystem::exists(fname)) {
                d_filename = fname;
                if (d_output_type != "message") {
                    // attempt to open for processing
                    this->open(fname.c_str(), d_repeat);
                }
            }
        }
    }

    d_method_count++;
    // if (d_method_count > d_file_queue_depth / 2) {
    //   GR_LOG_ERROR(d_logger,"too many messages in a row");
    // }
}

int file_source_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock lock(d_setlock);

    int size = noutput_items;
    int nread = 0;
    char* out = (char*)output_items[0];
    // std::cout << "noutput_items = " << noutput_items << std::endl;

    d_method_count--;

    if (not d_reader->is_open()) {
        // no file ready to be produced...sleep for a bit to avoid
        // consuming all resources
        usleep(1);
        return 0;
    }

    while (size) {
        // add stream tags if necessary
        if (d_tag_now) {
            d_tags = d_reader->get_tags();
            for (auto tag : d_tags) {
                add_item_tag(
                    0, nitems_written(0) + noutput_items - size, tag.key, tag.value);
            }

            // add repeat tag if specified
            if (not pmt::equal(d_add_begin_tag, pmt::PMT_NIL)) {
                add_item_tag(0,
                             nitems_written(0) + noutput_items - size,
                             d_add_begin_tag,
                             pmt::from_long(d_repeat_cnt));
            }

            // reset
            d_tag_now = false;
        }

        // read data
        nread = d_reader->read(out, size);

        // update pointers
        size -= nread;
        out += (nread * d_itemsize);

        if (size == 0) { // done
            break;
        }

        if ((nread > 0) and (d_reader->is_open())) { // short read, try again
            continue;
        }

        // We got a zero from read.  This is either EOF or an error.
        // In any event, if we're in repeat mode, seek back to the
        // beginning of the file and try again, otherwise load next file
        // and process
        if (not d_repeat) {
            // prepare next files
            if (d_reader->eof()) {
                d_reader->close();
                open_next();
            }

            // all done
            break;
        } else {
            d_reader->seek(0, SEEK_SET);
            d_repeat_cnt++;
        }
    } /* end while(size) */

    if (size > 0) {
        if (size == noutput_items) {
            // didn't read anything so say we're done
            return -1;
        } else {
            // return partial result
            return noutput_items - size;
        }
    } else {
        // produced all that was requested
        return noutput_items;
    }
} /* end work() */

} /* namespace sandia_utils */
} /* namespace gr */
