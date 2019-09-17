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

#ifndef INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H
#define INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H

#include <sandia_utils/complex_to_interleaved_short.h>

namespace gr {
  namespace sandia_utils {

    class complex_to_interleaved_short_impl : public complex_to_interleaved_short
    {
     private:
     float d_scale;
     bool d_vector;

     public:
      complex_to_interleaved_short_impl(bool vector, float scale);
      ~complex_to_interleaved_short_impl();

      void complex_array_to_interleaved_short(const gr_complex *in, short *out, int nsamples, float scale);
      void set_scale(float scale);

      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H */

