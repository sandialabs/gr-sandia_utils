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

#ifndef INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H
#define INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H

#include <sandia_utils/interleaved_short_to_complex.h>

namespace gr {
  namespace sandia_utils {

    class interleaved_short_to_complex_impl : public interleaved_short_to_complex
    {
     private:
      bool d_vector_input;
      bool d_swap;
      float d_scale;

      void interleaved_short_array_to_complex(const short *in, gr_complex *out, int nsamples, float scale);
    
      public:
      interleaved_short_to_complex_impl(bool vector_input, bool swap, float scale);
      ~interleaved_short_to_complex_impl();

      void set_swap(bool swap);
      void set_scale(float scale);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H */

