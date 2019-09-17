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
#include "interleaved_short_to_complex_impl.h"
#include <assert.h>
#include <volk/volk.h>

namespace gr {
  namespace sandia_utils {

    interleaved_short_to_complex::sptr
    interleaved_short_to_complex::make(bool vector_input, bool swap, float scale)
    {
      return gnuradio::get_initial_sptr
        (new interleaved_short_to_complex_impl(vector_input, swap, scale));
    }

    /*
     * The private constructor
     */
    interleaved_short_to_complex_impl::interleaved_short_to_complex_impl(bool vector_input, bool swap, float scale)
      : sync_decimator("interleaved_short_to_complex",
		       gr::io_signature::make (1, 1, (vector_input?2:1)*sizeof(short)),
		       gr::io_signature::make (1, 1, sizeof(gr_complex)), vector_input?1:2),
        d_vector_input(vector_input), d_swap(swap), d_scale(scale)
    {}

    /*
     * Our virtual destructor.
     */
    interleaved_short_to_complex_impl::~interleaved_short_to_complex_impl()
    {
    }

    void interleaved_short_to_complex_impl::set_swap(bool swap)
    {
		d_swap = swap;
    }

    void interleaved_short_to_complex_impl::set_scale(float scale)
    {
		d_scale = scale;
	  }

    // method to call the volk kernel
    void
    interleaved_short_to_complex_impl::interleaved_short_array_to_complex (const short *in,
                gr_complex *out, int nsamples, float scale)
    {
      assert (nsamples % 2 == 0);
      volk_16i_s32f_convert_32f_u( (float*)out, (const int16_t*) in, scale, nsamples );
    }

    int
    interleaved_short_to_complex_impl::work(int noutput_items,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
    {
      const short *in = (const short *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      interleaved_short_array_to_complex (in, out, 2 * noutput_items, d_scale);

      if (d_swap) {
        float* p = (float*)output_items[0];
        for (int i = 0; i < noutput_items; ++i) {
          float f = p[2*i+1];
          p[2*i+1] = p[2*i+0];
          p[2*i+0] = f;
        }
      }

      return noutput_items;
    }

  } /* namespace sandia_utils */
} /* namespace gr */

