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

#include "complex_to_interleaved_short_impl.h"
#include <gnuradio/io_signature.h>
#include <assert.h>
#include <volk/volk.h>

namespace gr {
namespace sandia_utils {

complex_to_interleaved_short::sptr complex_to_interleaved_short::make(bool vector,
                                                                      float scale)
{
    return gnuradio::get_initial_sptr(
        new complex_to_interleaved_short_impl(vector, scale));
}

/*
 * The private constructor
 */
complex_to_interleaved_short_impl::complex_to_interleaved_short_impl(bool vector,
                                                                     float scale)
    : gr::sync_interpolator(
          "complex_to_interleaved_short",
          io_signature::make(1, 1, sizeof(gr_complex)),
          io_signature::make(1, 1, vector ? 2 * sizeof(short) : sizeof(short)),
          vector ? 1 : 2),
      d_vector(vector),
      d_scale(scale)
{
}

/*
 * Our virtual destructor.
 */
complex_to_interleaved_short_impl::~complex_to_interleaved_short_impl() {}

void complex_to_interleaved_short_impl::set_scale(float scale) { d_scale = scale; }

// method to call the volk kernel
void complex_to_interleaved_short_impl::complex_array_to_interleaved_short(
    const gr_complex* in, short* out, int nsamples, float scale)
{
    assert(nsamples % 2 == 0);
    volk_32f_s32f_convert_16i((short*)out, (const float*)in, scale, nsamples);
}

int complex_to_interleaved_short_impl::work(int noutput_items,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    short* out = (short*)output_items[0];

    int n_shorts = (d_vector ? noutput_items * 2 : noutput_items);
    complex_array_to_interleaved_short(in, out, n_shorts, d_scale);

    return noutput_items;
}

} /* namespace sandia_utils */
} /* namespace gr */
