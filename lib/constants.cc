/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "gnuradio/sandia_utils/constants.h"


namespace gr {
namespace sandia_utils {

const pmt::pmt_t PMTCONSTSTR__time()
{
    static const pmt::pmt_t val = pmt::mp("time");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_time()
{
    static const pmt::pmt_t val = pmt::mp("rx_time");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__tx_time()
{
    static const pmt::pmt_t val = pmt::mp("tx_time");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__direction()
{
    static const pmt::pmt_t val = pmt::mp("direction");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__RX()
{
    static const pmt::pmt_t val = pmt::mp("RX");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__TX()
{
    static const pmt::pmt_t val = pmt::mp("TX");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__lo_freq()
{
    static const pmt::pmt_t val = pmt::mp("lo_freq");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__rate()
{
    static const pmt::pmt_t val = pmt::mp("rate");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__bandwidth()
{
    static const pmt::pmt_t val = pmt::mp("bandwidth");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_rate()
{
    static const pmt::pmt_t val = pmt::mp("rx_rate");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__rx_freq()
{
    static const pmt::pmt_t val = pmt::mp("rx_freq");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__sob()
{
    static const pmt::pmt_t val = pmt::mp("sob");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__eob()
{
    static const pmt::pmt_t val = pmt::mp("eob");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__pdu()
{
    static const pmt::pmt_t val = pmt::mp("pdu");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__fname()
{
    static const pmt::pmt_t val = pmt::mp("fname");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__in()
{
    static const pmt::pmt_t val = pmt::mp("in");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__out()
{
    static const pmt::pmt_t val = pmt::mp("out");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__tune()
{
    static const pmt::pmt_t val = pmt::mp("tune");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__overflow()
{
    static const pmt::pmt_t val = pmt::mp("overflow");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__BLOCK()
{
    static const pmt::pmt_t val = pmt::mp("BLOCK");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__pdu_in()
{
    static const pmt::pmt_t val = pmt::mp("pdu_in");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__pdu_out()
{
    static const pmt::pmt_t val = pmt::mp("pdu_out");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__energy()
{
    static const pmt::pmt_t val = pmt::mp("energy");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__power()
{
    static const pmt::pmt_t val = pmt::mp("power");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__dsp_freq()
{
    static const pmt::pmt_t val = pmt::mp("dsp_freq");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__burst_time()
{
    static const pmt::pmt_t val = pmt::mp("burst_time");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__tagged_bits_block()
{
    static const pmt::pmt_t val = pmt::mp("tagged_bits_block");
    return val;
}

const pmt::pmt_t PMTCONSTSTR__samples()
{
    static const pmt::pmt_t val = pmt::mp("samples");
    return val;
}

const pmt::pmt_t PMTCONSTSTR__item_size()
{
    static const pmt::pmt_t val = pmt::mp("item_size");
    return val;
}

const pmt::pmt_t PMTCONSTSTR__tune_request()
{
    static const pmt::pmt_t val = pmt::mp("tune_request");
    return val;
}

const pmt::pmt_t PMTCONSTSTR__publish()
{
    static const pmt::pmt_t val = pmt::mp("publish");
    return val;
}

const pmt::pmt_t PMTCONSTSTR__timeout()
{
    static const pmt::pmt_t val = pmt::mp("timeout");
    return val;
}



} // end namespace sandia_utils
} // end namespace gr
