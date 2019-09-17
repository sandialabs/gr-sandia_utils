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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "file_source_impl.h"


namespace gr {
  namespace sandia_utils {

    file_source::sptr
    file_source::make(size_t itemsize, const char *filename, const char *type,
      bool repeat, bool force_new)
    {
      return gnuradio::get_initial_sptr
        (new file_source_impl(itemsize,filename,type,repeat,force_new));
    }

    /*
     * The private constructor
     */
    file_source_impl::file_source_impl(size_t itemsize, const char *filename, const char *type,
      bool repeat, bool force_new)
      : gr::sync_block("file_source",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize),
        d_repeat(repeat),
        d_force_new(force_new),
        d_tag_on_open(false),
        d_repeat_cnt(0),
        d_add_begin_tag(pmt::PMT_NIL),
        d_tag_now(false),
        d_first_pass(true),
        d_file_queue_depth(DEFAULT_FILE_QUEUE_DEPTH),
        d_method_count(0)
    {
      // get reader
      if (strcmp(type,"raw") == 0) {
        d_reader = file_reader_base::sptr(new file_reader_base(itemsize, d_logger));
      }
      else if (strcmp(type,"raw_header")==0) {
        d_reader = file_reader_base::sptr(new file_reader_raw_header(itemsize, d_logger));
      }
#ifdef HAVE_BLUEFILE_LIB
      else if (strcmp(type,"bluefile")==0) {
        d_reader = file_reader_base::sptr(new file_reader_bluefile(itemsize, d_logger));
      }
#endif
      else {
        throw std::runtime_error(str(boost::format("Invalid file source format %s") % type));
      }

      // empty list of tags for now
      d_tags.resize(0);

      // open file if specified
      if (filename[0] != '\0') {
        this->open(filename,d_repeat);
      }

      // register message ports
      message_port_register_in(pmt::intern("pdu"));
      set_msg_handler(pmt::intern("pdu"), boost::bind(&file_source_impl::handle_msg, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    file_source_impl::~file_source_impl()
    {
      /* NOOP */
    }

    bool
    file_source_impl::seek(long seek_point, int whence)
    {
      return d_reader->seek(seek_point,whence);
    }


    void
    file_source_impl::open(const char *filename, bool repeat)
    {
      // update repeat status
      d_repeat = repeat;

      // only open if we are forcing open.  otherwise, place file onto
      // queue and let work function handle Opening
      if (d_force_new) {
        GR_LOG_DEBUG(d_logger,"Forcing file close and new file open");
        d_reader->close();
        d_reader->open(filename);
        d_tag_now = true;
      }
      else {
        if (d_file_queue.size() >= d_file_queue_depth) {
          // modifynig queue so protect
          gr::thread::scoped_lock lock(fp_mutex);

          // clear queue
          GR_LOG_ERROR(d_logger, "Maximum number of file entries reached...resetting");
          while (d_file_queue.size()) { d_file_queue.pop(); }

          // add new file and tag
          d_file_queue.push(std::make_pair(std::string(filename), true));
        }
        else {
          // add new file and tag if necessary
          bool add_tags = d_first_pass or d_tag_now or d_tag_on_open;
          d_file_queue.push(std::make_pair(std::string(filename),add_tags));
          d_first_pass = false;
        }
      }

      // attempt to open next files
      open_next();
    }

    void
    file_source_impl::open_next()
    {
      if ((not d_reader->is_open()) and (d_file_queue.size())) {
        // obtain exclusive access for duration of this scope
        gr::thread::scoped_lock lock(fp_mutex);

        std::pair<std::string, bool> value = d_file_queue.front();
        d_reader->open(value.first.c_str());
        d_tag_now = value.second;

        // remove file
        d_file_queue.pop();
        // std::cout << d_file_queue.size() << std::endl;
      }
    }

    void
    file_source_impl::close()
    {
      d_reader->close();
    }

    void
    file_source_impl::set_begin_tag(pmt::pmt_t tag)
    {
      d_add_begin_tag = tag;
      // d_tag_now = not pmt::equal(d_add_begin_tag, pmt::PMT_NIL);
    }

    void
    file_source_impl::handle_msg(pmt::pmt_t pdu) {
      // is_pair() will pass both dictionaries and pairs (possible PDUs...)
      if (!pmt::is_pair(pdu)) {
        return;
      }

      try {
        // Will fail if msg is not a dictionary or PDU (dict/pmt pair)
        pmt::pmt_t x = pmt::dict_keys(pdu);
      } catch (const pmt::wrong_type &e) {
        return;
      }

      // we have a dictionary or a pair of which one element is a dict
      pmt::pmt_t fname = pmt::dict_ref(pdu, FNAME_KEY, pmt::PMT_NIL);
      if (not pmt::equal(fname,pmt::PMT_NIL)) {
        this->open(pmt::symbol_to_string(fname).c_str(),d_repeat);
      }

      d_method_count++;
      // if (d_method_count > d_file_queue_depth / 2) {
      //   GR_LOG_ERROR(d_logger,"too many messages in a row");
      // }
    }

    int
    file_source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      int size = noutput_items;
      int nread = 0;
      char *out = (char *)output_items[0];
      // std::cout << "noutput_items = " << noutput_items << std::endl;

      // obtain exclusive access for duration of this function
      // gr::thread::scoped_lock lock(fp_mutex);
      d_method_count--;

      if (not d_reader->is_open()) {
        // no file ready to be produced...sleep for a bit to avoid
        // consuming all resources
        usleep(1);
        return 0;
      }

      while (size) {
        // add stream tags if necessary
        if (d_tag_now) {
          d_tags = d_reader->get_tags();
          for (auto tag : d_tags) {
            add_item_tag(0,nitems_written(0) + noutput_items - size, tag.key, tag.value);
          }

          // add repeat tag if specified
          if (not pmt::equal(d_add_begin_tag, pmt::PMT_NIL)) {
            add_item_tag(0,nitems_written(0) + noutput_items - size,
              d_add_begin_tag, pmt::from_long(d_repeat_cnt));
          }

          // reset
          d_tag_now = false;
        }

        // read data
        nread = d_reader->read(out,size);

        // update pointers
        size -= nread;
        out += (nread * d_itemsize);

        if (size == 0) { // done
          break;
        }

        if ((nread > 0) and (d_reader->is_open())){ // short read, try again
          continue;
        }

        // We got a zero from read.  This is either EOF or an error.
        // In any event, if we're in repeat mode, seek back to the
        // beginning of the file and try again, otherwise load next file
        // and process
        if (not d_repeat) {
          // prepare next files
          if (d_reader->eof()) {
            d_reader->close();
            open_next();
          }
          else if (d_reader->is_open()) { continue; }
          else { break; }
        }
        else {
          d_reader->seek(0,SEEK_SET);
          d_repeat_cnt++;
        }
      } /* end while(size) */

      if (size > 0) {
        if (size == noutput_items) {
          // didn't read anything so say we're done
          return -1;
        }
        else {
          // return partial result
          return noutput_items - size;
        }
      }
      else {
        // produced all that was requested
        return noutput_items;
      }
    } /* end work() */

  } /* namespace sandia_utils */
} /* namespace gr */
