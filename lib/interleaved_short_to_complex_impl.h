/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

    void interleaved_short_array_to_complex(const short* in,
                                            gr_complex* out,
                                            int nsamples,
                                            float scale);

public:
    interleaved_short_to_complex_impl(bool vector_input, bool swap, float scale);
    ~interleaved_short_to_complex_impl();

    void set_swap(bool swap);
    void set_scale(float scale);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_IMPL_H */
