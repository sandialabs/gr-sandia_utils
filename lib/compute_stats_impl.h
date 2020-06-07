/* -*- c++ -*- */
/*
 * Copyright 2020 gr-sandia_utils author.
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

#ifndef INCLUDED_SANDIA_UTILS_COMPUTE_STATS_IMPL_H
#define INCLUDED_SANDIA_UTILS_COMPUTE_STATS_IMPL_H

#include <sandia_utils/compute_stats.h>

namespace gr {
namespace sandia_utils {

class compute_stats_impl : public compute_stats
{
private:
    pmt::pmt_t PMT_ENERGY = pmt::mp("energy");
    pmt::pmt_t PMT_POWER = pmt::mp("power");
    pmt::pmt_t PMT_PDU_OUT = pmt::mp("pdu_out");

public:
    compute_stats_impl();
    ~compute_stats_impl();

    // Where all the action really happens
    void handle_pdu(pmt::pmt_t pdu);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_COMPUTE_STATS_IMPL_H */
