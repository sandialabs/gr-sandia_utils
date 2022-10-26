/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifndef INCLUDED_SANDIA_UTILS_CONSTANTS_H
#define INCLUDED_SANDIA_UTILS_CONSTANTS_H

#include <gnuradio/sandia_utils/api.h>
#include <pmt/pmt.h>

namespace gr {
namespace sandia_utils {


SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__time();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_time();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__tx_time();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__direction();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__RX();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__TX();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__lo_freq();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__rate();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__bandwidth();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_rate();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__rx_freq();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__sob();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__eob();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__pdu();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__fname();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__in();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__out();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__tune();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__overflow();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__BLOCK();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__pdu_in();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__pdu_out();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__energy();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__power();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__dsp_freq();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__burst_time();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__tagged_bits_block();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__samples();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__item_size();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__publish();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__tune_request();
SANDIA_UTILS_API const pmt::pmt_t PMTCONSTSTR__timeout();

enum STUB_MODE { DROP_STUB = 0, PAD_RIGHT = 1, PAD_LEFT = 2 };
enum GATE_STATE { GATE_WAIT, GATE_DISCARD, GATE_PUBLISH };

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_CONSTANTS_H */
