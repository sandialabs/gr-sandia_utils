/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_H
#define INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_H

#include <gnuradio/sync_decimator.h>
#include <sandia_utils/api.h>

#include <gnuradio/sync_interpolator.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Convert complex data stream to interleaved short data stream
 * \ingroup sandia_utils
 *
 * This block is a clone of the gnuradio complex_to_ishort, but using a
 * Volk kernel with the scaling parameter enabled. Eventually this should live
 * in gr_blocks instead of here.
 *
 */
class SANDIA_UTILS_API complex_to_interleaved_short : virtual public gr::sync_interpolator
{
public:
    typedef std::shared_ptr<complex_to_interleaved_short> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * sandia_utils::complex_to_interleaved_short.
     *
     * To avoid accidental use of raw pointers,
     * sandia_utils::complex_to_interleaved_short's constructor is in a private
     * implementation class. sandia_utils::complex_to_interleaved_short::make is the
     * public interface for creating new instances.
     */
    static sptr make(bool vector = false, float scale = 1.0);

    /*! \brief Set the scaling factor applied to each short output sample
     *
     * \param scale Scale factor
     */
    virtual void set_scale(float scale) = 0;
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_COMPLEX_TO_INTERLEAVED_SHORT_H */
