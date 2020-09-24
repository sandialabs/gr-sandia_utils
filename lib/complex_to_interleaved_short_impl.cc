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
