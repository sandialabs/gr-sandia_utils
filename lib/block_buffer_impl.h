/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_IMPL_H
#define INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_IMPL_H

#include <sandia_utils/block_buffer.h>
#include <mutex>
#include <queue>

namespace gr {
  namespace sandia_utils {

    class block_buffer_impl : public block_buffer
    {
     private:

      // block parameters
      size_t      d_itemsize;
      uint64_t    d_nsamples;
      uint64_t    d_next_nsamples;

      // reserve space in the output buffer to guarantee calls to work
      bool        d_init;
      int         d_nreserved;

      // buffer indices
      uint64_t    d_write_idx, d_read_idx;

      // used to insert rx_time tags when samples are skipped
      float       d_samp_rate;
      uint64_t    d_last_abs_read_idx;
      tag_t       d_current_rx_time_tag;
      std::queue<tag_t> d_rx_time_tags;

      // which of the three buffers is currently being written to or read from
      int         d_reading;
      int         d_writing;
      int         d_latest;
      bool        d_latest_valid;

      // pass data?
      bool        d_pass_data;
      
      // input buffer size
      int 	  d_input_buff_size;
      
      // spinlock timeouts (computed based on input buffer size and sample rate)
      int 	  d_usleep;

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

      const pmt::pmt_t PMT_RX_RATE = pmt::mp("rx_rate");
      const pmt::pmt_t PMT_RX_TIME = pmt::mp("rx_time");
      const pmt::pmt_t PMT_OVERFLOW = pmt::mp("overflow");
      const pmt::pmt_t PMT_BLOCK   = pmt::mp("BLOCK");

     public:
      block_buffer_impl(size_t itemsize, uint64_t nsamples, float samp_rate,
        bool pass_data = true);
      ~block_buffer_impl();

      void set_pass_data(bool pass_data) {
        std::lock_guard<std::mutex> lock(work_mutex);
        d_pass_data = pass_data;
      }

      bool get_pass_data() { return d_pass_data; }

      void set_nsamples(uint64_t nsamples);

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_IMPL_H */
