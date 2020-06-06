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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file_writer_base.h"
#include "file_writer_raw.h"
#include "file_writer_raw_header.h"
#ifdef HAVE_BLUEFILE_LIB
#include "file_writer_bluefile.h"
#endif
#include "pmt/pmt.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>     // pow
#include <string>     // stoi

namespace fs = boost::filesystem;

namespace gr {
  namespace sandia_utils {

    // String substitution
    std::string
    strrepl(std::string f, std::string s, int value, int divisor, std::string spec = "%d")
    {
      // populate local variables first
      char buff[20];
      std::string temp = f;

      // replace all instances of fc (center frequency)
      std::size_t found = temp.find(s);
      while (found != std::string::npos){
        sprintf(buff,spec.c_str(),value / divisor);
        temp = temp.substr(0,found) + buff + temp.substr(found+s.size());
        found = temp.find(s);
      }

      return temp;
    }

    file_writer_base::sptr
    file_writer_base::make(std::string data_type, std::string file_type,
                         size_t itemsize, uint64_t nsamples, int rate,
                         std::string out_dir, std::string name_spec,
                         gr::logger_ptr logger)
    {
      sptr p;
      if (file_type == "raw"){
        p = sptr(new file_writer_raw(data_type, file_type, itemsize,
          nsamples, rate, out_dir, name_spec));
      }
      else if (file_type == "raw_header") {
        p = sptr(new file_writer_raw_header(data_type, file_type, itemsize,
          nsamples, rate, out_dir, name_spec));
      }
#ifdef HAVE_BLUEFILE_LIB
      else if (file_type == "bluefile"){
        p = sptr(new file_writer_bluefile(data_type, file_type, itemsize,
          nsamples, rate, out_dir, name_spec));
      }
#endif
      else
        throw std::runtime_error("file_sink:invalid file type");

      // initialize all members
      p->d_data_type = data_type;
      p->d_file_type = file_type;
      p->d_itemsize = itemsize;
      p->d_rate = rate;
      p->d_out_dir = out_dir;
      p->d_name_spec = name_spec;
      p->d_name_spec_base = name_spec;
      p->d_new_folder = false;
      p->d_freq = 0;
      p->d_nwritten = 0;
      p->d_nwritten_total = 0;

      // initialize logger - use stdout if not specified
      if (logger == NULL) { p->d_logger = gr::logger_ptr(&std::cout);}
      else { p->d_logger = logger; }

      // set number of samples
      p->set_nsamples(nsamples);

      // set flag
      p->d_is_started = false;

      return p;
    }

    void
    file_writer_base::start(epoch_time start_time)
    {
      d_samp_time = start_time;
      d_samp_time_next = d_samp_time;
      d_samp_time_next += d_T;

      // setup counters
      d_nwritten = 0;
      d_nwritten_total = 0;
      d_nremaining = d_nsamples;

      // generate folder if necessary
      gen_folder(start_time);

      // generate base filename
      gen_filename_base();

      // open file
      d_filename = gen_filename();
      open(d_filename);

      // set flag
      d_is_started = true;
    }

    void file_writer_base::stop()
    {
      // use virtual method to properly close file
      close();

      // Increment file number
      d_file_num++;
      if (d_file_num_rollover > 0) { d_file_num %= (uint64_t)d_file_num_rollover; }

      // signal for update to be sent only if data has been written
      if (d_nwritten) {
        d_callback(d_filename,d_samp_time,double(d_freq),double(d_rate));
      }

      // clear file currently being written
      d_filename = "";

      // reset number of samples written for the current file
      d_nwritten = 0;

      // clear flag
      d_is_started = false;
    }

    void
    file_writer_base::write(const void *in, int nitems)
    {
      uint64_t nleft = (uint64_t)nitems;
      char *p = reinterpret_cast<char *>(const_cast<void *>(in));
      if (d_nsamples){
        while(nleft)
        {
          uint64_t ntowrite = std::min(d_nremaining, nleft);

          // write samples
          uint64_t nwritten = (uint64_t)write_impl((void *)p,ntowrite);
          d_nwritten += nwritten;
          d_nwritten_total += nwritten;
          p += (ntowrite*d_itemsize);

          // update
          nleft -= ntowrite;
          d_nremaining -= ntowrite;
          if (d_nremaining == 0){
            boost::recursive_mutex::scoped_lock lock(d_lock);

            // close file currently being processed
            stop();

            // reset
            d_nremaining = d_nsamples;

            // update sample time
            d_samp_time = d_samp_time_next;

            // generate next file and open
            d_filename = gen_filename();
            open(d_filename);

            // reset flag
            d_is_started = true;

            // set next sample time
            d_samp_time_next += d_T;
          }
        }
      }
      else
      {
        // single file - write all samples
        uint64_t nwritten = (uint64_t)write_impl((void *)p,nitems);
        d_nwritten += nwritten;
        d_nwritten_total += nwritten;
      }
    }

    void
    file_writer_base::gen_folder(epoch_time& start_time)
    {
      // if directory not specified, set to current path
      if (d_out_dir.empty()) { d_out_dir = fs::current_path().string(); }

      // ensure base output path exists
      fs::path temp_dir = fs::path(d_out_dir);
      if (not fs::is_directory(temp_dir))
      {
        // TODO: Provide better debug information
//         throw std::runtime_error(boost::format("Invalid output path %s") % d_out_dir);
        throw std::runtime_error("Invalid output path");
      }

      if (d_new_folder){
        // TODO: Allow user to specify
        // current folder naming convention is:
        // YYYYMMDD/HH_MM_SS

        // get utc start time
        time_t sample_second = (time_t)d_samp_time.epoch_sec();
        struct tm *gmt_start_time;
        gmt_start_time = gmtime(&sample_second);

        // generate day folder
        char day_str[9];
        strftime(day_str,9,"%Y%m%d",gmt_start_time);

        // generate time folder
        char time_str[9];
        strftime(time_str,9,"%H_%M_%S",gmt_start_time);

        // generate full path and crate if it does not exist
        d_full_out_path = temp_dir / std::string(day_str) / std::string(time_str);
        if (not fs::is_directory(d_full_out_path))
        {
          if (not fs::create_directories(d_full_out_path))
          {
            throw std::runtime_error("unable to generate output folder");
          }
        }
      }
      else { d_full_out_path = temp_dir; }
    } /* end gen_folder */

    void
    file_writer_base::gen_filename_base()
    {
      /**********************************************************************
       * Generate base file name
       *
       * Certain parameters can not change during a recording so we will
       * generate the basename to avoid having to replace every specifier
       * for each generated filename
       *********************************************************************/
      // reset name spec
      d_name_spec = d_name_spec_base;

      d_name_spec = strrepl(d_name_spec,"%fcM",d_freq,1000000);
      d_name_spec = strrepl(d_name_spec,"%fck",d_freq,1000);
      d_name_spec = strrepl(d_name_spec,"%fcc",d_freq,1);
      d_name_spec = strrepl(d_name_spec,"%fsM",d_rate,1000000);
      d_name_spec = strrepl(d_name_spec,"%fsk",d_rate,1000);
      d_name_spec = strrepl(d_name_spec,"%fsc",d_rate,1);

    } /* end gen_filename_base */

    std::string
    file_writer_base::gen_filename()
    {
      // TODO: Move all this to run only once and replace the modulo file number
      // only

      // update basic file number specifier
      std::string fname = strrepl(d_name_spec,"%fd",d_file_num % 100000,1,"%05d");

      // update extended file number specifier
      std::string temp = fname;
      std::size_t found = temp.find('%');
      while (found != std::string::npos) {
        std::size_t end_ind = fname.find("fd",found);
        size_t next_found = fname.find('%',found+1);

        // build modifier based on number of files
        if ((end_ind != std::string::npos) and (end_ind < next_found)) {
          // string to replace
          std::string modifier = fname.substr(found,end_ind+2-found);

          // get number of files to process and generate replacement string
          std::string repl = "%";
          if (fname.find("0",found+1,1) != std::string::npos) {
            repl += "0";
            found++;
          }

          try {
            // catch errors in parsing
            std::string file_mod = fname.substr(found+1,end_ind-found-1);
            int nchars = stoi(file_mod);
            repl += (file_mod + "d");
            fname = strrepl(fname,modifier,
              d_file_num % int(pow(10.0,nchars)),1,repl);
          }
          catch(...) { /* NOOP */}

          break;
        }

        // look for next occurence
        found = next_found;
      }

      // file time
      time_t sample_second = (time_t)d_samp_time.epoch_sec();
      struct tm *gmt_start_time;
      gmt_start_time = gmtime(&sample_second);

      // populate date/time values
      char time_temp[100];
      strftime(time_temp,100,fname.c_str(),gmt_start_time);

      // generate output file
      fs::path outfile = d_full_out_path / std::string(time_temp);
      return (outfile.string());

    } /* end gen_filename */
  } /* namespace sandia_utils */
} /* namespace gr */
