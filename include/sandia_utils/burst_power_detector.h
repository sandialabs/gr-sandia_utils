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


#ifndef INCLUDED_SANDIA_UTILS_BURST_POWER_DETECTOR_H
#define INCLUDED_SANDIA_UTILS_BURST_POWER_DETECTOR_H

#include <gnuradio/sync_block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Time-domain, power-based burst detection
 * \ingroup sandia_utils
 *
 * Burst detection using instantaneous averaged power and a single-sided
 * constant false-alarm rate (CFAR) detector.  A low-pass, moving-average
 * filter is applied to the incoming complex data stream.  SOB and EOB
 * tags are added to the data stream to denote the start and end of a burst.
 * Caution:  This is not yet completely functional
 */
class SANDIA_UTILS_API burst_power_detector : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<burst_power_detector> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::burst_power_detector.
     *
     * To avoid accidental use of raw pointers, sandia_utils::burst_power_detector's
     * constructor is in a private implementation
     * class. sandia_utils::burst_power_detector::make is the public interface for
     * creating new instances.
     *
     * \param naverage    Number of instantaneous power samples to average
     * \param nguard      Number of samples between test points
     * \param threshold   Detection threshold (dB)
     * \param holdoff     Number of samples to include before and after burst edges
     */
    static sptr make(int naverage, int nguard, double threshold, int holdoff);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_BURST_POWER_DETECTOR_H */
