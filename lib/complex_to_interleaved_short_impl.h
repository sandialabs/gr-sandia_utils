/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

    void complex_array_to_interleaved_short(const gr_complex* in,
                                            short* out,
                                            int nsamples,
                                            float scale);
    void set_scale(float scale);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_IMPL_H */
