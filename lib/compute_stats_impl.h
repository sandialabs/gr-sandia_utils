/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_COMPUTE_STATS_IMPL_H
#define INCLUDED_SANDIA_UTILS_COMPUTE_STATS_IMPL_H

#include <gnuradio/sandia_utils/compute_stats.h>

namespace gr {
namespace sandia_utils {

class compute_stats_impl : public compute_stats
{
private:

public:
    compute_stats_impl();
    ~compute_stats_impl();

    // Where all the action really happens
    void handle_pdu(pmt::pmt_t pdu);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_COMPUTE_STATS_IMPL_H */
