/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#ifndef INCLUDED_SANDIA_UTILS_FILE_WRITER_BASE_H
#define INCLUDED_SANDIA_UTILS_FILE_WRITER_BASE_H

#include <string>
#include <stdint.h>           /* uint64_t */
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <sandia_utils/api.h>
#include <pmt/pmt.h>
#include <gnuradio/logger.h>
#include "epoch_time.h"

namespace gr {
  namespace sandia_utils {
    class SANDIA_UTILS_API file_writer_base
    {
    public:
      typedef boost::shared_ptr<file_writer_base> sptr;
      typedef boost::function<void(std::string,epoch_time,double,double)> callback;

      /*!
       * \brief Return a shared_ptr to a new instance of sandia_utils::file_sink_base.
       *
       * To avoid accidental use of raw pointers, sandia_utils::file_sink_base's
       * constructor is in a private implementation
       * class. sandia_utils::file_sink_base::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string data_type, std::string file_type,
                       size_t itemsize, uint64_t nsamples, int rate,
                       std::string out_dir, std::string name_spec,
                        gr::logger_ptr logger);

       /*!
        * \brief Register update callback
        *
        * Register a method to be called upon file completion
        *
        * param callback Callback function
        */
       void register_callback(callback callback) { d_callback = callback; };

       /*!
        * \brief Get current filename
        *
        * Get the current file being written
        */
       std::string get_filename() { return d_filename; }

      /*!
       * \brief Generate new folder
       *
       * Upon starting of the writer, a new folder can be generated
       * that the data will be placed into.  Otherwise, data will be
       * written to the current directory
       */
      void set_gen_new_folder(bool value) { d_new_folder = value; }

      /*!
       * \brief Generate new folder
       *
       * Upon starting of the writer, a new folder can be generated
       * that the data will be placed into.  Otherwise, data will be
       * written to the current directory
       */
      bool get_gen_new_folder() { return d_new_folder; }

      /*!
       * \brief Set center frequency
       *
       */
      void set_freq(uint64_t freq) { d_freq = freq; }

      /*!
       * \brief GSet center frequency
       *
       */
      uint64_t get_freq() { return d_freq; }

      /*!
       * \brief Set sampling rate
       *
       */
      void set_rate(int rate) {
        boost::recursive_mutex::scoped_lock lock(d_mutex);

        d_rate = rate;
        d_T = (double)d_nsamples / (double)d_rate;
      }

      /*!
       * \brief Get current sampling rate
       *
       */
      int get_rate() { return d_rate; }

      /*!
       * \brief Set number of samples per file
       *
       */
      void set_nsamples(uint64_t nsamples) {
        boost::recursive_mutex::scoped_lock lock(d_mutex);

        d_nsamples = nsamples;
        d_T = (double)d_nsamples / (double)d_rate;
      }

      /*!
       * \brief Get number of samples per file
       *
       */
      int get_nsamples() { return d_nsamples; }

      /*!
       * \brief Set file number rollover
       *
       */
      void set_file_num_rollover(int rollover) {
        boost::recursive_mutex::scoped_lock lock(d_mutex);

        d_file_num_rollover = rollover;
      }

      /*!
       * \brief Get file number rollover
       *
       */
      int get_file_num_rollover() { return d_file_num_rollover; }

      /*!
       * \brief Start file writer
       *
       * Start the file writer.
       */
      void start(uint64_t start_sec, double start_frac = 0.0) {
        this->start(epoch_time(start_sec,start_frac));
      }
      void start(epoch_time start_time);

      /*!
       * \brief Determine if file writer has been started
       *
       */
      bool is_started() { return d_is_started; }

      /*!
       * \brief Stop file writer
       *
       * Stop the file writer.
       */
      void stop();

      /*!
       * \brief Write data
       *
       */
      void write(const void *in, int nitems);

      /*!
       * \brief Open file for processing
       *
       */
      virtual void open(std::string fname) = 0;

      /*!
       * \brief Close file for writing
       *
       */
      virtual void close() = 0;

      /*!
       * \brief Write data to file
       *
       * Start the file writer.
       */
      virtual int write_impl(const void *in, int nitems) = 0;

    protected:
      // file name_spec
      std::string           d_filename;
      // data type string
      // allowed values: TODO
      std::string           d_data_type;

      // file type string
      std::string           d_file_type;

      // size of each item
      size_t                d_itemsize;

      // number of samples per file (0 = infinite)
      uint64_t              d_nsamples;

      // sample rate
      int                   d_rate;

      // file sample period (seconds)
      double                d_T;

      // center frequency
      uint64_t                   d_freq;

      // base output directory
      std::string           d_out_dir;

      // updated output directory
      boost::filesystem::path  d_full_out_path;

      // name specification
      std::string           d_name_spec;
      std::string           d_name_spec_base;

      // generate new folder on start
      bool                  d_new_folder;

      // time of first sample
      epoch_time                d_samp_time;
      epoch_time                d_samp_time_next;

      // file number
      uint64_t              d_file_num = 0;

      // number of samples written and remaining
      uint64_t              d_nwritten_total;
      uint64_t              d_nwritten;
      uint64_t              d_file_size;
      uint64_t              d_nremaining;

      // thread safe access
      boost::recursive_mutex          d_mutex;

      // update callback
      callback d_callback;

      // logger
      gr::logger_ptr            d_logger;

      // state flag
      bool                      d_is_started;

      // file number rollover - value less than 0 indicates no rollover
      int                       d_file_num_rollover;

    private:
      void gen_folder(epoch_time& start_time);
      void gen_filename_base();
      std::string gen_filename();

      // thread-safe locking
      boost::recursive_mutex  d_lock;

    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_WRITER_IMPL_H */
