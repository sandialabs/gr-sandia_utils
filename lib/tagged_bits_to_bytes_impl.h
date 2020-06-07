/* -*- c++ -*- */
/*
 * Copyright 2020 gr-sandia_utils author.
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

#ifndef INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_IMPL_H
#define INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_IMPL_H

#include <sandia_utils/tagged_bits_to_bytes.h>

namespace gr {
namespace sandia_utils {

class tagged_bits_to_bytes_impl : public tagged_bits_to_bytes
{
    struct time_pair {
        uint64_t sec;
        double frac;
    };

private:
    pmt::pmt_t d_pmt_tag_key;
    bool d_lsb_first;
    int d_stub_mode;
    int d_vlen;
    double d_sample_rate;
    size_t d_itemsize;
    int d_extra_bits;

    pmt::pmt_t d_rxtime_tag = pmt::intern("rx_time");
    pmt::pmt_t d_pmt_burst_time = pmt::intern("burst_time");
    pmt::pmt_t d_pmt_tagged_bits_block = pmt::intern("tagged_bits_block");

    time_pair d_last_rx_time;
    uint64_t d_last_rx_time_offset;
    std::vector<tag_t> d_tags;

    uint16_t packbits(uint8_t* bits, uint32_t length);
    size_t
    packbuffer(const uint8_t* in, size_t length, uint8_t* out_array, size_t nbytes_align);
    time_pair get_start_time(uint64_t this_offset);

public:
    tagged_bits_to_bytes_impl(std::string key,
                              bool little_endian,
                              int stub_mode,
                              int v_len,
                              double sample_rate);
    ~tagged_bits_to_bytes_impl();

    // getters/setters
    void set_vlen(int vlen);
    void set_little_endian_flag(bool lsb_first);
    void set_sample_rate(double samp_rate);

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_IMPL_H */
