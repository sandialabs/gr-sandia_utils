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

#include "invert_tune_impl.h"
#include "gnuradio/sandia_utils/constants.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace sandia_utils {

invert_tune::sptr invert_tune::make()
{
    return gnuradio::get_initial_sptr(new invert_tune_impl());
}

/*
 * The private constructor
 */
invert_tune_impl::invert_tune_impl()
    : gr::block("invert_tune",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_n_error(0),
      d_n_exception(0),
      d_n_pdu(0),
      d_n_ops(0)
{
    message_port_register_in(PMTCONSTSTR__tune());
    set_msg_handler(PMTCONSTSTR__tune(),
                    [this](pmt::pmt_t msg) { this->handle_msg(msg); });
    message_port_register_out(PMTCONSTSTR__tune());
}

/*
 * Our virtual destructor.
 */
invert_tune_impl::~invert_tune_impl() {}

/*
 * This goes here as print statements in the destructor are usually not shown
 */
bool invert_tune_impl::stop()
{
    if (d_n_ops) {
        GR_LOG_DEBUG(d_logger,
                     boost::format("invert_tune_impl::stop(), Operations %d") % d_n_ops);
    }
    if (d_n_error) {
        GR_LOG_DEBUG(d_logger,
                     boost::format("invert_tune_impl::stop(), errors %d") % d_n_error);
    }
    if (d_n_pdu) {
        GR_LOG_DEBUG(d_logger,
                     boost::format("invert_tune_impl::stop(), pdus %d") % d_n_pdu);
    }
    if (d_n_exception) {
        GR_LOG_DEBUG(d_logger,
                     boost::format("invert_tune_impl::stop(), exceptions %d") %
                         d_n_exception);
    }

    return true;
}


void invert_tune_impl::handle_msg(pmt::pmt_t msg)
{
    bool is_pdu = false;
    pmt::pmt_t meta = pmt::PMT_NIL;
    pmt::pmt_t cdr = pmt::PMT_NIL;
    if (pmt::is_dict(msg)) {
        meta = msg;
    } else if (pmt::is_pdu(msg) or pmt::is_pair(msg)) {
        is_pdu = true;
        meta = pmt::car(msg);
        cdr = pmt::cdr(msg);
    } else {
        d_n_error++;
        return;
    }

    // update metadata dictionary if dsp frequency is specified
    if (pmt::dict_has_key(meta, PMTCONSTSTR__dsp_freq())) {
        pmt::pmt_t freq_pmt = pmt::dict_ref(meta, PMTCONSTSTR__dsp_freq(), pmt::PMT_NIL);
        double freq(pmt::to_double(freq_pmt));
        meta = pmt::dict_add(meta, PMTCONSTSTR__dsp_freq(), pmt::from_double(freq * -1));
    }

    // publish using original type
    if (is_pdu) {
        message_port_pub(PMTCONSTSTR__tune(), pmt::cons(meta, cdr));
    } else {
        message_port_pub(PMTCONSTSTR__tune(), meta);
    }
}

} /* namespace sandia_utils */
} /* namespace gr */
