/* -*- c++ -*- */
/* 
 * Copyright 2019 gr-sandia_utils author.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "tagged_bits_to_bytes_impl.h"
#include "sandia_utils/constants.h"

namespace gr {
  namespace sandia_utils {

    tagged_bits_to_bytes::sptr
    tagged_bits_to_bytes::make(std::string key, bool little_endian, int stub_mode, int v_len, double sample_rate)
    {
      return gnuradio::get_initial_sptr
        (new tagged_bits_to_bytes_impl(key, little_endian, stub_mode, v_len, sample_rate));
    }

    /*
     * The private constructor
     */
    tagged_bits_to_bytes_impl::tagged_bits_to_bytes_impl(std::string key, bool little_endian, int stub_mode, int v_len, double sample_rate)
      : gr::block("tagged_bits_to_bytes",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(uint8_t)*v_len))
    {
      // key to filter on
      d_pmt_tag_key = pmt::intern(key);

      // endianess flag for packing
      d_lsb_first = little_endian;

      // output bytes in vectors
      d_vlen = v_len;
      d_itemsize = 8*v_len;

      // keep track of how many bits we had to pad/drop to get alignment correct
      d_extra_bits = 0;

      // sample rate is used for time tag propagation
      d_sample_rate = sample_rate;

      // the last time tag
      d_last_rx_time.sec = 0;
      d_last_rx_time.frac = 0.0;

      // stub mode is what to do with a chunk less that 8 bits long
      //   0 - DROP
      //   1 - Pad Right
      //   2 - Pad Left
      d_stub_mode = stub_mode;

      // tag propagation will need to be manual
      set_tag_propagation_policy(TPP_DONT);
      // this is the rate if there were no drops/pads
      //set_relative_rate(1.0/(8.0*d_vlen));
    }

    /*
     * Our virtual destructor.
     */
    tagged_bits_to_bytes_impl::~tagged_bits_to_bytes_impl()
    {
    }

    void
    tagged_bits_to_bytes_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items*8; // causes locked up tests...
      //ninput_items_required[0] = noutput_items;
    }

    void
    tagged_bits_to_bytes_impl::set_vlen(int vlen) {
      d_vlen = vlen;
      d_itemsize = 8*vlen;
    }

    void
    tagged_bits_to_bytes_impl::set_little_endian_flag(bool lsb_first) {
      d_lsb_first = lsb_first;
    }

    void
    tagged_bits_to_bytes_impl::set_sample_rate(double samp_rate) {
      d_sample_rate = samp_rate;
    }


    uint16_t
    tagged_bits_to_bytes_impl::packbits(uint8_t *bits, uint32_t length) {
      uint16_t packed = 0;
      uint32_t nbits = length;

      if (nbits > 16) {
        std::cout << "ERROR: cannot pack length " << length  << " bits!" << std::endl;
      } else {
        packed += bits[0];
        for (int ii = 1; ii < nbits; ii++) {
          packed <<= 1;
          packed += bits[ii];
        }
      }
      return packed;
    }

    size_t
    tagged_bits_to_bytes_impl::packbuffer(const uint8_t *in, size_t length, uint8_t *out_array, size_t nbytes_align) {
      /*
       * PACK BYTE: input is an aray of bits to be converted to bytes. The
       * input should be a multiple of 8 bytes and  will be zero padded to
       * multiple of 8 bytes (same behavior as numpy.packbits() has).
       * Values other than zero will map to '1'.
       */
      std::vector<uint8_t> data(in, in+length);
      std::vector<uint8_t> out;

      uint32_t nbytes = 0;
      uint32_t last_data_byte_idx = 0;

      // what should we do with chunks smaller than 8 bits? Mode selected
      if (d_stub_mode == DROP_STUB) {
        nbytes = data.size() / 8; // round down (DROP small chunks) bits
        nbytes -= nbytes % nbytes_align; // round down bytes
      }
      else {
        nbytes = (data.size()+7) / 8; // round up bits
        last_data_byte_idx = nbytes - 1; // save this sample index for later adjustment
        nbytes += (nbytes_align - (nbytes % nbytes_align)) % nbytes_align; // round upbytes
      }

      data.resize(nbytes*8, 0);
      out.reserve(nbytes);

      uint8_t byte = 0;
      for (int ii = 0; ii < data.size(); ii++) {
        if (d_lsb_first) {
          // START LSB FIRST PROCESSING
          byte >>= 1;
          if (data[ii]) byte |= 0x80;
            if ((ii % 8)==7) {
            out.push_back(byte);
            byte = 0;
          }
          // END LSB FIRST PROCESSING
        } else {
          // START MSB FIRST PROCESSING
          byte <<= 1;
          if (data[ii]) byte |=0x01;
            if ((ii % 8)==7) {
            out.push_back(byte);
            byte = 0;
          }
          // END MSB FIRST PROCESSING
        }
      }

      // pad left is the weird case. length-number of zeros were right padded, flip that
      if ((d_stub_mode == PAD_LEFT) and (length%8 != 0)){
        out[last_data_byte_idx] >>= (8-(length%8));
      }

      // copy to the given array
      std::copy(out.begin(), out.end(), out_array);

      // return how many bytes were made
      return out.size();
    }

    tagged_bits_to_bytes_impl::time_pair
    tagged_bits_to_bytes_impl::get_start_time(uint64_t this_offset) {
      /* determine the time. we always have the offset that the SOB tag
       * was received on, we know the rate, and also have a known time/offset
      * pair - calculate the delta and apply accordingly.
      */
      double delta;
      delta = ((double)this_offset - (double)d_last_rx_time_offset)/ d_sample_rate;

      int int_delta = (int)delta;
      delta -= int_delta;

      uint64_t int_seconds = d_last_rx_time.sec + int_delta;
      double frac_seconds = d_last_rx_time.frac + delta;

      // what if outside 1.0?
      if (frac_seconds >= 1.0) {
        frac_seconds -= 1.0;
        int_seconds += 1;
      } else if (frac_seconds < 0) {
        frac_seconds += 1.0;
        int_seconds -= 1;
      }

      time_pair start_time = {int_seconds, frac_seconds};
      return start_time;
    }

    int
    tagged_bits_to_bytes_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];
      uint8_t *out = (uint8_t *) output_items[0];

      uint64_t rel_start = 0;
      uint64_t rel_end = ninput_items[0];
      //uint64_t rel_end = noutput_items;
      //uint64_t rel_offset = (rel_end/d_itemsize) * d_itemsize;
      uint64_t rel_offset = rel_end - rel_end % d_itemsize;

      //////////////////////////////////////////////////////////
      // Time tags
      //////////////////////////////////////////////////////////
      // update the last rx_time tag if there are any in these samples
      get_tags_in_window(d_tags, 0, rel_start, rel_offset, d_rxtime_tag);
      if (d_tags.size()) {
        tag_t last_tag = d_tags.back();
        d_last_rx_time.sec = pmt::to_uint64(pmt::tuple_ref(last_tag.value, 0));
        d_last_rx_time.frac = pmt::to_double(pmt::tuple_ref(last_tag.value, 1));
        d_last_rx_time_offset = last_tag.offset;
      }
      d_tags.clear();

      //////////////////////////////////////////////////////////
      // Handle Burst location cases
      //////////////////////////////////////////////////////////
      // look for all BURST tags, one at position zero is expected/ok
      std::vector<tag_t> tags;
      get_tags_in_window(tags, 0, rel_start, rel_offset, d_pmt_tag_key);

      uint64_t abs_idx = nitems_read(0);
      // if there is a single tag at offset 0, we are aligned
      if (tags.size() == 1 and tags[0].offset == abs_idx) {
        //std::cout << "single tag at rel offset zero" << std::endl;
        //rel_offset = (rel_end/(8*d_vlen))*(8*d_vlen); // already set above
      }

      // if there is at least a single tag, consume up until it, leave the rest for the next work() call
      else if (tags.size() >= 1 and tags[0].offset != abs_idx) {
        //std::cout << "first tag NOT at rel offset zero" << std::endl;
        rel_offset = tags[0].offset - abs_idx;
      } 

      // multiple tags, first one is at zero, consume from the first to the second
      else if (tags.size() > 1 and tags[0].offset == abs_idx) {
        //std::cout << "multiple tags, first one at rel offset zero" << std::endl;
        rel_offset = tags[1].offset - abs_idx;
      }

      //////////////////////////////////////////////////////////
      // Pack into bytes
      //////////////////////////////////////////////////////////
      size_t nbytes_produced = this->packbuffer(&in[0], rel_offset, out, d_vlen);

      //////////////////////////////////////////////////////////
      // Tag propagation
      //////////////////////////////////////////////////////////
      // calculate both of these even though we will only ever care about one
      //int nbits_dropped = rel_offset%8;
      //int nbits_added = (8-(rel_offset%8))%8;
      int nbits_dropped = rel_offset % d_itemsize;
      int nbits_added = (d_itemsize - nbits_dropped) % d_itemsize;

      if (d_stub_mode == DROP_STUB and nbits_dropped) {
        //std::cout << "bits that got dropped: " << (nbits_dropped) << std::endl;
        d_extra_bits -= nbits_dropped;
      }

      else if (d_stub_mode != DROP_STUB and nbits_added){
        //std::cout << "bits that got added: " << (nbits_added) << std::endl;
        d_extra_bits += nbits_added;
      }
      //d_extra_bits = 0;
      //std::cout << "d_extra_bits is "  << d_extra_bits << std::endl;

      // manual tag propagation so we can handle the case when we skip/drop bits
      std::vector<tag_t> alltags;
      get_tags_in_window(alltags, 0, rel_start, rel_offset);
      for (auto tag : alltags) {
        // correct the tag offset on all tags
        uint64_t new_offset = (tag.offset + d_extra_bits) / d_itemsize;
        //std::cout << "moving tag from " << tag.offset << " to " << new_offset*d_vlen << std::endl;
        tag.offset = new_offset;

        // rx_time tags will be time-shifted to the beginning of the byte
        if (pmt::equal(tag.key, d_rxtime_tag)){
          // read in the original time
          uint64_t int_sec = pmt::to_uint64(pmt::tuple_ref(tag.value, 0));
          double frac_sec  = pmt::to_double(pmt::tuple_ref(tag.value, 1));

          // shift the time to the beginning of that byte boundary
          int nbits_shifted = tag.offset % d_itemsize;
          double timeshift = nbits_shifted / d_sample_rate;
          frac_sec -= timeshift;
          if (frac_sec < 0){
            int_sec -= 1;
            frac_sec += 1;
          }

          // correct the tag value with the new time
          pmt::pmt_t updated_time = pmt::make_tuple(pmt::from_uint64(int_sec), pmt::from_double(frac_sec));
          tag.value = updated_time;
        }
          
        //add_item_tag(0, new_offset, tag.key, tag.value, pmt::intern("debug_sam_this_one"));
        add_item_tag(0, tag);
      }

      // tag the BURST sample with time for later use
      if ((rel_offset != 0) and (tags.size()) and (tags[0].offset == abs_idx) ){
        time_pair this_time = get_start_time(tags[0].offset);
        pmt::pmt_t time_pair_pmt = pmt::make_tuple(pmt::from_uint64(this_time.sec), pmt::from_double(this_time.frac));
        add_item_tag(0, nitems_written(0), d_pmt_burst_time, time_pair_pmt, d_pmt_tagged_bits_block);
      }

      //////////////////////////////////////////////////////////
      // Clean up and return
      //////////////////////////////////////////////////////////
      // sleep to avoid spin-locking if we can't consume
      if (rel_offset == 0) {
        usleep(100);
      }

      // consume up to the first tag only, not including the tagged sample
      this->consume(0,rel_offset);

      // produce number of packed bytes
      return nbytes_produced / d_vlen;
    }

  } /* namespace sandia_utils */
} /* namespace gr */

