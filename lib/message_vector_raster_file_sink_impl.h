/* -*- c++ -*- */
/* 
 * Copyright 2018 gr-sandia_utils author.
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

#ifndef INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_IMPL_H
#define INCLUDED_SANDIA_UTILS_MESSAGE_VECTOR_RASTER_FILE_SINK_IMPL_H

#include <sandia_utils/message_vector_raster_file_sink.h>
#include <pmt/pmt.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include<string>

namespace gr {
  namespace sandia_utils {

    class message_vector_raster_file_sink_impl : public message_vector_raster_file_sink
    {
     private:
       pmt::pmt_t       d_mp_name;

       // file variables
       std::string      d_filename;
       std::string      d_filename_tmp;
       bool             d_file_is_new;

       // time
       boost::posix_time::ptime t1;
       boost::posix_time::ptime t2;
       boost::posix_time::time_duration diff;
       int update_rate;

       // buffer to store rows
       std::vector<uint8_t> d_buffer;
       uint16_t d_rows;
       uint16_t d_total_rows;

       // protection mutex
       boost::mutex     d_mutex;

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

