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

      // message hop period
      int           d_msg_hop_period;

      // output type
      std::string         d_output_type;

      // current file being processed
      std::string         d_filename;

      // message thread objects
      boost::shared_ptr<gr::thread::thread> d_thread;
      bool d_finished;

     public:
      /**
       * Constructor
       *
       * @param itemsize - per item size in bytes
       * @param filename - filename to open as source.
       * @param type - type of file input, Example Values = message, raw, raw_header, bluefile
       * @param repeat - repeat a single file over and over.
       * @param force_new - Force open new file upon command, regardless of current status
       */
      file_source_impl(size_t itemsize, const char *filename, const char *type,
        bool repeat = false, bool force_new = false);
      virtual ~file_source_impl();

      // overloaded block functions
      bool start();
      bool stop();

      /**
       * Seek in the file source
       *
       * @param seek_point - offset position to seek to
       * @param whence - refrence point for seek, see fseek
       * @return bool - 0 or false on success
       */
      bool seek(long seek_point, int whence);

      /**
       * manages opening a file
       * If d_force_new, file is opened immediately
       * If !d_force_new, file is added to the queue for later playing
       *
       * @param filename - file to open
       * @param repeat - set the repeat mode.
       */
      void open(const char *filename, bool repeat);

      void close();

      /**
       * Gnu Radio entry point to perform work
       *
       * @param noutput_items - number of items that should be provided
       * @param input_items -
       * @param output_items -  storage for output items
       * @return int - number of items produced
       */
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);

      void set_begin_tag(pmt::pmt_t val);

      void set_file_queue_depth(size_t depth) {
        d_file_queue_depth = depth;
      }

      void add_file_tags(bool tag) { d_tag_on_open = tag; }

      void set_msg_hop_period(int period_ms);

     private:
      /**
       * Handles incoming PDUs to the PDU command port. Incoming PDUs
       * can be used to command a file open.
       *
       * Incomig PDU must contain a PMT dict that has a key entry of "fname".
       * The associated value of "fname" will be used as the filename to open.
       *
       * @param pdu - incoming PDU to process
       */
       void handle_msg(pmt::pmt_t pdu);

       /**
        * Pop's and opens the next file from the file queue
        */
       void open_next();

       /**
        * Thread function for message source
        */
       void run();
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_SOURCE_IMPL_H */
