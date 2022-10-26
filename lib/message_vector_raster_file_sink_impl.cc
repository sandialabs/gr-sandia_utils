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

#include "message_vector_raster_file_sink_impl.h"
#include "gnuradio/sandia_utils/constants.h"
#include <gnuradio/io_signature.h>
#include <cstdio> /* rename */
#include <exception>
#include <fstream>

namespace gr {
namespace sandia_utils {

message_vector_raster_file_sink::sptr
message_vector_raster_file_sink::make(std::string filename, int n_rows)
{
    return gnuradio::get_initial_sptr(
        new message_vector_raster_file_sink_impl(filename, n_rows));
}

/*
 * The private constructor
 */
message_vector_raster_file_sink_impl::message_vector_raster_file_sink_impl(
    std::string filename, int n_rows)
    : gr::block("message_vector_raster_file_sink",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_filename(filename),
      d_file_is_new(false),
      d_total_rows(n_rows),
      d_rows(0)
{
    // generate temporary filename
    d_filename_tmp = d_filename + ".tmp";

    // clear buffer
    d_buffer.clear();

    // get time
    t1 = boost::posix_time::microsec_clock::local_time();
    t2 = boost::posix_time::microsec_clock::local_time();
    update_rate = 100; // milliseconds

    // register message port
    message_port_register_in(PMTCONSTSTR__in());
    set_msg_handler(PMTCONSTSTR__in(), [this](pmt::pmt_t msg) { this->handle_msg(msg); });
}

/*
 * Our virtual destructor.
 */
message_vector_raster_file_sink_impl::~message_vector_raster_file_sink_impl() {}

void message_vector_raster_file_sink_impl::reset()
{
    boost::mutex::scoped_lock lock(d_mutex);

    d_buffer.clear();
    d_rows = 0;
}

void message_vector_raster_file_sink_impl::handle_msg(pmt::pmt_t msg)
{
    boost::mutex::scoped_lock lock(d_mutex);
    try {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t car = pmt::car(msg);
            pmt::pmt_t cdr = pmt::cdr(msg);

            bool is_ready = false;
            if (pmt::is_uniform_vector(cdr)) {
                size_t nbytes;
                void* p = pmt::uniform_vector_writable_elements(cdr, nbytes);

                // store in a buffer
                d_buffer.insert(d_buffer.end(), (uint8_t*)p, (uint8_t*)p + nbytes);
                d_rows++;
                if (d_rows > d_total_rows) {
                    d_buffer.erase(d_buffer.begin(), d_buffer.begin() + nbytes);
                    d_rows--;
                    is_ready = true;
                }

                // check how long it's been since we've written a file
                t2 = boost::posix_time::microsec_clock::local_time();
                diff = t2 - t1;

                // if it's been a while, write a new file when ready
                if ((diff.total_milliseconds() > update_rate) and is_ready) {
                    // update last time
                    t1 = boost::posix_time::microsec_clock::local_time();

                    // write to file
                    std::ofstream f(d_filename_tmp, std::ios::out | std::ios::binary);
                    if (f.is_open()) {
                        // f.write((const char *)&d_buffer[0], d_buffer.size());
                        f.write((const char*)&d_buffer[0], d_buffer.size());
                        f.close();
                    }

                    // move file
                    rename(d_filename_tmp.c_str(), d_filename.c_str());

                    // signal a file is new
                    d_file_is_new = true;
                }
            }
        }
    } catch (const std::exception& e) {
        /* NOOP */
    }

    return;
}

std::string message_vector_raster_file_sink_impl::get_filename()
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
