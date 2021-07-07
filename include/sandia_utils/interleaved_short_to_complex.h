/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_H
#define INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_H

#include <gnuradio/sync_decimator.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Convert stream of interleaved shorts to a stream of complex
 * with scaling factor.
 * \ingroup sandia_utils
 *
 * This block is a clone of the gnuradio ishort_to_complex, but with the
 * scaling parameter enabled. Eventually this should live in gr_blocks
 * instead of here.
 *
 */
class SANDIA_UTILS_API interleaved_short_to_complex : virtual public gr::sync_decimator
{
public:
    typedef std::shared_ptr<interleaved_short_to_complex> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * sandia_utils::interleaved_short_to_complex.
     *
     * To avoid accidental use of raw pointers,
     * sandia_utils::interleaved_short_to_complex's constructor is in a private
     * implementation class. sandia_utils::interleaved_short_to_complex::make is the
     * public interface for creating new instances.
     */
    static sptr make(bool vector_input = false, bool swap = false, float scale = 1.0);

    /*! \brief Swap I/Q samples
     *
     * \param swap Swap I/Q samples
     */
    virtual void set_swap(bool swap) = 0;

    /*! \brief Set the scaling factor applied to each short sample before
     * deinterleaving and forming a complex sample
     *
     * \param scale Scale factor
     */
    virtual void set_scale(float scale) = 0;
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_H */
