/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_FILE_SINK_IMPL_H
#define INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_FILE_SINK_IMPL_H

#include <gnuradio/sandia_utils/message_vector_file_sink.h>
#include <pmt/pmt.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <string>

namespace gr {
namespace sandia_utils {

class message_vector_file_sink_impl : public message_vector_file_sink
{
private:
    // file variables
    std::string d_filename;
    bool d_append;
    std::ofstream d_fid;

    // protection mutex
    boost::mutex d_mutex;

public:
    message_vector_file_sink_impl(std::string filename, bool append = false);
    ~message_vector_file_sink_impl();

    // overloaded methods
    bool start();
    bool stop();

    void handle_msg(pmt::pmt_t);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_FILE_SINK_IMPL_H */
