/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_IMPL_H
#define INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_IMPL_H

#include <gnuradio/sandia_utils/block_buffer.h>
#include <mutex> // std::mutex
#include <queue> // std::queue

namespace gr {
namespace sandia_utils {

class block_buffer_impl : public block_buffer
{
private:
    // block parameters
    size_t d_itemsize;
    uint64_t d_nsamples;
    uint64_t d_next_nsamples;

    // reserve space in the output buffer to guarantee calls to work
    bool d_init;
    int d_nreserved;

    // buffer indices
    uint64_t d_write_idx, d_read_idx;

    // used to insert rx_time tags when samples are skipped
    float d_samp_rate;
    uint64_t d_last_abs_read_idx;
    tag_t d_current_rx_time_tag;
    std::queue<tag_t> d_rx_time_tags;

    // which of the three buffers is currently being written to or read from
    int d_reading;
    int d_writing;
    int d_latest;
    bool d_latest_valid;

    // pass data?
    bool d_pass_data;

    // input buffer size
    int d_input_buff_size;

    // spinlock timeouts (computed based on input buffer size and sample rate)
    int d_usleep;

    // internal buffers
    struct {
        void* ptr = nullptr;
        std::vector<tag_t> tags;
        uint64_t abs_read_idx;
        uint64_t abs_write_idx;
        pmt::pmt_t rx_time;
    } d_buf[3];

    // make set_nsamples and general_work threadsafe
    std::mutex work_mutex;

    void init_buffers(uint64_t nsamples);

public:
    block_buffer_impl(size_t itemsize,
                      uint64_t nsamples,
                      float samp_rate,
                      bool pass_data = true);
    ~block_buffer_impl();

    void set_pass_data(bool pass_data)
    {
        std::lock_guard<std::mutex> lock(work_mutex);
        d_pass_data = pass_data;
    }

    bool get_pass_data() { return d_pass_data; }

    void set_nsamples(uint64_t nsamples);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_IMPL_H */
