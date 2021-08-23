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
#include <gnuradio/io_signature.h>
#include <cstdio> /* rename */
#include <exception>
#include <fstream>

namespace gr {
namespace sandia_utils {

message_vector_file_sink::sptr message_vector_file_sink::make(std::string filename)
{
    return gnuradio::get_initial_sptr(new message_vector_file_sink_impl(filename));
}

/*
 * The private constructor
 */
message_vector_file_sink_impl::message_vector_file_sink_impl(std::string filename)
    : gr::block("message_vector_file_sink",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_filename(filename),
      d_mp_name(pmt::mp("in")),
      d_file_is_new(false)
{
    // generate temporary filename
    d_filename_tmp = d_filename + ".tmp";

    message_port_register_in(d_mp_name);
    set_msg_handler(d_mp_name,
                    boost::bind(&message_vector_file_sink_impl::handle_msg, this, boost::placeholders::_1));
}

/*
 * Our virtual destructor.
 */
message_vector_file_sink_impl::~message_vector_file_sink_impl() {}

void message_vector_file_sink_impl::handle_msg(pmt::pmt_t msg)
{
    boost::mutex::scoped_lock lock(d_mutex);
    try {
        if (pmt::is_pair) {
            pmt::pmt_t car = pmt::car(msg);
            pmt::pmt_t cdr = pmt::cdr(msg);

            if (pmt::is_uniform_vector(cdr)) {
                size_t nbytes;
                void* p = pmt::uniform_vector_writable_elements(cdr, nbytes);

                // write to file
                std::ofstream f(d_filename_tmp, std::ios::out | std::ios::binary);
                if (f.is_open()) {
                    f.write((const char*)p, nbytes);
                    f.close();
                }

                // move file
                rename(d_filename_tmp.c_str(), d_filename.c_str());

                // signal a file is new
                d_file_is_new = true;
            }
        }
    } catch (const std::exception& e) {
        /* NOOP */
    }

    return;
}

std::string message_vector_file_sink_impl::get_filename()
{
    boost::mutex::scoped_lock lock(d_mutex);

    if (d_file_is_new) {
        d_file_is_new = false;
        return d_filename;
    } else {
        return "";
    }
}
} /* namespace sandia_utils */
} /* namespace gr */
