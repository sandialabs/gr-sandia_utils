/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
