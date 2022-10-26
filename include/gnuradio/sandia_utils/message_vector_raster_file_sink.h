/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_H
#define INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_H

#include <gnuradio/sandia_utils/api.h>
#include <gnuradio/sync_block.h>
#include <string>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Save the most recent messages to a file at periodic intervals
 * \ingroup sandia_utils
 *
 * The message vector raster file sink blocks processes incoming PDUs
 * and stores a set number of uniform vectors before writing to the
 * specified file.  Once the required number of messages has been received
 * and the update time has elapsed, the file is generated.
 *
 * The file is not generated until the number of messages required is
 * received, no matter the update rate. Only the last \p n_rows messsages
 * are retained, so if more than \p n_rows messages are received during an
 * update period, only the latest are saved to the file.
 *
 */
class SANDIA_UTILS_API message_vector_raster_file_sink : virtual public gr::block
{
public:
    typedef std::shared_ptr<message_vector_raster_file_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * sandia_utils::message_vector_raster_file_sink.
     *
     * To avoid accidental use of raw pointers,
     * sandia_utils::message_vector_raster_file_sink's constructor is in a private
     * implementation class. sandia_utils::message_vector_raster_file_sink::make is the
     * public interface for creating new instances.
     */
    static sptr make(std::string filename, int n_rows);

    /*!
     * \brief Reset raster
     */
    virtual void reset() = 0;

    /*!
     * \brief Get the last file produced.
     *
     * \return File name if generated, otherwise an empty string ""
     */
    virtual std::string get_filename() = 0;
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_H */
