/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_IMPL_H
#define INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_IMPL_H

#include <gnuradio/sandia_utils/message_vector_raster_file_sink.h>
#include <pmt/pmt.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <string>

namespace gr {
namespace sandia_utils {

class message_vector_raster_file_sink_impl : public message_vector_raster_file_sink
{
private:

    // file variables
    std::string d_filename;
    std::string d_filename_tmp;
    bool d_file_is_new;

    // buffer to store rows
    std::vector<uint8_t> d_buffer;
    uint16_t d_total_rows;
    uint16_t d_rows;

    // time
    boost::posix_time::ptime t1;
    boost::posix_time::ptime t2;
    boost::posix_time::time_duration diff;
    int update_rate;

    // protection mutex
    boost::mutex d_mutex;

public:
    message_vector_raster_file_sink_impl(std::string filename, int n_rows);
    ~message_vector_raster_file_sink_impl();

    std::string get_filename();
    void reset();
    void handle_msg(pmt::pmt_t);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_IMPL_H */
