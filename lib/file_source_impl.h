/* -*- c++ -*- */
/*
 * Copyright 2019 gr-sandia_utils author.
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

#ifndef INCLUDED_SANDIA_UTILS_FILE_SOURCE_IMPL_H
#define INCLUDED_SANDIA_UTILS_FILE_SOURCE_IMPL_H

#include <queue>
#include <utility>
#include <gnuradio/tags.h>
#include <sandia_utils/file_source.h>
#include <sandia_utils/constants.h>
#include <boost/thread/mutex.hpp>
#include "file_reader_base.h"
#include "file_reader_raw_header.h"

// conditional libraries
#ifdef HAVE_BLUEFILE_LIB
  #include "file_reader_bluefile.h"
#endif

#define DEFAULT_FILE_QUEUE_DEPTH 100

namespace gr {
  namespace sandia_utils {

    class file_source_impl : public file_source
    {
     private:
      size_t  d_itemsize;
      bool  d_repeat;
      bool  d_force_new;
      int   d_repeat_cnt;
      bool  d_first_pass;
      pmt::pmt_t    d_add_begin_tag;

      boost::mutex fp_mutex;

      // reader object
      file_reader_base::sptr d_reader;

      // file queue (filename, tag flag)
      std::queue<std::pair<std::string, bool>>   d_file_queue;
      size_t  d_file_queue_depth;

      // add output tags
      bool          d_tag_now;
      std::vector<gr::tag_t>  d_tags;
      bool          d_tag_on_open;

      // debug stuff
      int           d_method_count;

     public:
      file_source_impl(size_t itemsize, const char *filename, const char *type,
        bool repeat = false, bool force_new = false);
      ~file_source_impl();

      bool seek(long seek_point, int whence);
      void open(const char *filename, bool repeat);
      void close();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void set_begin_tag(pmt::pmt_t val);

      void set_file_queue_depth(size_t depth) {
        d_file_queue_depth = depth;
      }

      void add_file_tags(bool tag) { d_tag_on_open = tag; }

     private:
       void handle_msg(pmt::pmt_t pdu);
       void open_next();    // get next file to be processed
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_SOURCE_IMPL_H */
