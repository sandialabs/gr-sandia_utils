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

#include "message_vector_file_sink_impl.h"
#include "gnuradio/sandia_utils/constants.h"
#include <gnuradio/io_signature.h>
#include <cstdio> /* rename */
#include <exception>
#include <fstream>

namespace gr {
namespace sandia_utils {

message_vector_file_sink::sptr message_vector_file_sink::make(std::string filename,
                                                              bool append)
{
    return gnuradio::get_initial_sptr(
        new message_vector_file_sink_impl(filename, append));
}

/*
 * The private constructor
 */
message_vector_file_sink_impl::message_vector_file_sink_impl(std::string filename,
                                                             bool append)
    : gr::block("message_vector_file_sink",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_filename(filename),
      d_append(append)
{
    message_port_register_in(PMTCONSTSTR__in());
    set_msg_handler(PMTCONSTSTR__in(), [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

/*
 * Our virtual destructor.
 */
message_vector_file_sink_impl::~message_vector_file_sink_impl() {}

bool message_vector_file_sink_impl::start()
{
    // open output file only once if appending incoming data
    if (d_append) {
        d_fid.open(d_filename, std::ios::out | std::ios::binary);
    }

    return true;
}

bool message_vector_file_sink_impl::stop()
{
    // close output file if open
    if (d_fid.is_open()) {
        d_fid.close();
    }

    return true;
}

void message_vector_file_sink_impl::handle_msg(pmt::pmt_t msg)
{
    boost::mutex::scoped_lock lock(d_mutex);
    try {
        if (pmt::is_pdu(msg)) {
            pmt::pmt_t car = pmt::car(msg);
            pmt::pmt_t cdr = pmt::cdr(msg);

            // extract uniform data
            size_t nbytes;
            void* p = pmt::uniform_vector_writable_elements(cdr, nbytes);

            if (not d_append) {
                // open output file anew so it is written over
                d_fid.open(d_filename, std::ios::out | std::ios::binary);
                d_fid.write((const char*)p, nbytes);
                d_fid.close();
            } else {
                // write number of samples and then data
                uint32_t nsamples =
                    (uint32_t)(nbytes / pmt::uniform_vector_itemsize(cdr));
                d_fid.write((const char*)&nsamples, sizeof(uint32_t));
                d_fid.write((const char*)p, nbytes);
            }
        }
    } catch (const std::exception& e) {
        /* NOOP */
    }

    return;
}

} /* namespace sandia_utils */
} /* namespace gr */
