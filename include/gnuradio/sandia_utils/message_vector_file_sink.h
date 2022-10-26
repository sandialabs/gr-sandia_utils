/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_FILE_SINK_H
#define INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_FILE_SINK_H

#include <gnuradio/sandia_utils/api.h>
#include <gnuradio/sync_block.h>
#include <string>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Save message vector data to file
 * \ingroup sandia_utils
 *
 * Save message vector data to file.  Input mesage must be a PDU with the
 * second element containing a uniform vector.  The output data is written
 * using the format of the generating system.
 *
 * If the append option is selected, all messages received between block
 * start and stop will be included in the output file.  In this instance, a
 * 4-byte unsigned integer will be prepended to the beginning of every data
 * block to indicate how many samples are included for the block.  The file
 * format is then:
 *
 *  |--------|-------------------|-------|--------------------------|
 *  |   N0   |   data0           |   N1  |        data1             |
 *  |--------|-------------------|-------|--------------------------|
 *  |   4    |  N0*sizeof(data)  |   4   |   N1*sizeof(data)        |
 *  |--------|-------------------|-------|--------------------------|
 *
 * If the append option is not selected, no 4-byte field will be prepended
 * and only the data from the last message received will be included in the
 * output file
 */
class SANDIA_UTILS_API message_vector_file_sink : virtual public gr::block
{
public:
    typedef std::shared_ptr<message_vector_file_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * sandia_utils::message_vector_file_sink.
     *
     * To avoid accidental use of raw pointers, sandia_utils::message_vector_file_sink's
     * constructor is in a private implementation
     * class. sandia_utils::message_vector_file_sink::make is the public interface for
     * creating new instances.
     *
     * \param filename Name of file to be generated
     * \param append Append new messages to end of file
     */
    static sptr make(std::string filename, bool append = false);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_FILE_SINK_H */
