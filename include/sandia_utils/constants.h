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

#include <pmt/pmt.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {


   static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("time");
   static const pmt::pmt_t RX_TIME_KEY = pmt::string_to_symbol("rx_time");
   static const pmt::pmt_t TX_TIME_KEY = pmt::string_to_symbol("tx_time");
   static const pmt::pmt_t CMD_DIRECTION = pmt::mp("direction");
   static const pmt::pmt_t CMD_RX = pmt::mp("RX");
   static const pmt::pmt_t CMD_TX = pmt::mp("TX");
   static const pmt::pmt_t CMD_LO_FREQ_KEY = pmt::mp("lo_freq");
   static const pmt::pmt_t CMD_RATE_KEY = pmt::mp("rate");
   static const pmt::pmt_t CMD_BW_KEY = pmt::mp("bandwidth");

   static const pmt::pmt_t RATE_KEY = pmt::string_to_symbol("rx_rate");
   static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("rx_freq");
   static const pmt::pmt_t BURST_START_KEY = pmt::string_to_symbol("sob");
   static const pmt::pmt_t BURST_STOP_KEY = pmt::string_to_symbol("eob");
   static const pmt::pmt_t PDU_KEY = pmt::string_to_symbol("pdu");
   static const pmt::pmt_t FNAME_KEY = pmt::string_to_symbol("fname");
   static const pmt::pmt_t IN_KEY = pmt::string_to_symbol("in");
   static const pmt::pmt_t OUT_KEY = pmt::string_to_symbol("out");
   static const pmt::pmt_t TUNE_KEY = pmt::string_to_symbol("tune");

enum STUB_MODE { DROP_STUB = 0, PAD_RIGHT = 1, PAD_LEFT = 2 };

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_CONSTANTS_H */
