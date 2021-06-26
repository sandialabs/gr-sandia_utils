/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_COMPUTE_STATS_H
#define INCLUDED_SANDIA_UTILS_COMPUTE_STATS_H

#include <gnuradio/block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Calculates Power and Energy in PDUs
 * \ingroup sandia_utils
 *
 * Operates on c32vector PDUs and calculates energy and power present.
 * Adds two dict entries, energy & power, with the calculated results
 * c32vector data is unmodified
 *
 */
class SANDIA_UTILS_API compute_stats : virtual public gr::block
{
public:
    typedef std::shared_ptr<compute_stats> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::compute_stats.
     *
     * To avoid accidental use of raw pointers, sandia_utils::compute_stats's
     * constructor is in a private implementation
     * class. sandia_utils::compute_stats::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_COMPUTE_STATS_H */
