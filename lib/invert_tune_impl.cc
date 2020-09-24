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
      d_n_ops(0),
      d_mp_name(pmt::mp("tune")),
      d_freq_key(pmt::mp("dsp_freq"))
{
    message_port_register_in(d_mp_name);
    set_msg_handler(d_mp_name, boost::bind(&invert_tune_impl::handle_msg, this, _1));
    message_port_register_out(d_mp_name);
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

    try {
        if (pmt::is_dict(msg)) {
            pmt::pmt_t freq_pmt = pmt::dict_ref(msg, d_freq_key, pmt::PMT_NIL);
            if (pmt::is_real(freq_pmt)) {
                double freq(pmt::to_double(freq_pmt));
                pmt::pmt_t tune_out =
                    pmt::dict_add(msg, d_freq_key, pmt::from_double(freq * -1));
                d_n_ops++;

                message_port_pub(d_mp_name, tune_out);
            } else {
                // maybe the message was a PDU, try extracting metadata
                handle_msg(pmt::car(msg));
                d_n_pdu++;
                return;
            }
        } else {
            d_n_error++;
        }
    } catch (...) {
        d_n_exception++;
    }
}

} /* namespace sandia_utils */
} /* namespace gr */
