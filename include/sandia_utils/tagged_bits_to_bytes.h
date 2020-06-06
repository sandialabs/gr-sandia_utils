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


#ifndef INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_H
#define INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_H

#include <sandia_utils/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace sandia_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup sandia_utils
     *
     */
    class SANDIA_UTILS_API tagged_bits_to_bytes : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<tagged_bits_to_bytes> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of sandia_utils::tagged_bits_to_bytes.
       *
       * To avoid accidental use of raw pointers, sandia_utils::tagged_bits_to_bytes's
       * constructor is in a private implementation
       * class. sandia_utils::tagged_bits_to_bytes::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string key="BURST", bool little_endian=false, int stub_mode=0, int v_len=1, double sample_rate = 1);

      virtual void set_vlen(int vlen)=0;
      virtual void set_little_endian_flag(bool lsb_first)=0;
      virtual void set_sample_rate(double samp_rate)=0;
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_H */

