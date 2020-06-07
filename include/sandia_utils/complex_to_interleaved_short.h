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
    typedef boost::shared_ptr<complex_to_interleaved_short> sptr;

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
