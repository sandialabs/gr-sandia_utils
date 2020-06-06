/* -*- c++ -*- */
/*
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "invert_tune_impl.h"

namespace gr {
  namespace sandia_utils {

    invert_tune::sptr
    invert_tune::make()
    {
      return gnuradio::get_initial_sptr
        (new invert_tune_impl());
    }

    /*
     * The private constructor
     */
    invert_tune_impl::invert_tune_impl()
      : gr::block("invert_tune",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        d_n_error(0),
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
    invert_tune_impl::~invert_tune_impl()
    {
    }

    /*
     * This goes here as print statements in the destructor are usually not shown
     */
    bool
    invert_tune_impl::stop()
    {
      if(d_n_error) {
        GR_LOG_INFO(d_logger, boost::format("invert tune block destroyed, had %d errors") % d_n_error);
      } else {
        GR_LOG_INFO(d_logger, "invert tune block destroyed without errors");
      }
      return true;
    }


    void
    invert_tune_impl::handle_msg(pmt::pmt_t msg) {

      try {
        if (pmt::is_dict(msg)) {
          pmt::pmt_t freq_pmt = pmt::dict_ref(msg, d_freq_key, pmt::PMT_NIL);
          if (pmt::is_real(freq_pmt)) {
            double freq(pmt::to_double(freq_pmt));
            pmt::pmt_t tune_out = pmt::dict_add(msg, d_freq_key, pmt::from_double(freq * -1));
            message_port_pub(d_mp_name, tune_out);
          } else {
            // maybe the message was a PDU, try extracting metadata
            handle_msg(pmt::car(msg));
            d_n_error++;
            return;
          }
        } else {
          d_n_error++;
        }
      } catch (...) {
        d_n_error++;
      }
    }

  } /* namespace sandia_utils */
} /* namespace gr */
