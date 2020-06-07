/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_SANDIA_UTILS_CONSTANTS_H
#define INCLUDED_SANDIA_UTILS_CONSTANTS_H

#include <pmt/pmt.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("rx_time");
static const pmt::pmt_t RATE_KEY = pmt::string_to_symbol("rx_rate");
static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("rx_freq");
static const pmt::pmt_t BURST_START_KEY = pmt::string_to_symbol("sob");
static const pmt::pmt_t BURST_STOP_KEY = pmt::string_to_symbol("eob");
static const pmt::pmt_t PDU_KEY = pmt::string_to_symbol("pdu");
static const pmt::pmt_t FNAME_KEY = pmt::string_to_symbol("fname");
static const pmt::pmt_t IN_KEY = pmt::string_to_symbol("in");
static const pmt::pmt_t OUT_KEY = pmt::string_to_symbol("out");

enum STUB_MODE { DROP_STUB = 0, PAD_RIGHT = 1, PAD_LEFT = 2 };

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_CONSTANTS_H */
