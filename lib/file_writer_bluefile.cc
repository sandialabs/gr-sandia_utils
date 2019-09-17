/* -*- c++ -*- */
/*
 * Copyright 2017 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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

#include "file_writer_bluefile.h"

#include <iostream>
#include <stdio.h>


namespace gr {
  namespace sandia_utils {
    file_writer_bluefile::file_writer_bluefile(std::string data_type, std::string file_type,
                                 size_t itemsize, uint64_t nsamples, int rate,
                                 std::string out_dir, std::string name_spec)
    {
      // set to null
      d_writer = NULL;
      d_initialized = false;
    }

    file_writer_bluefile::~file_writer_bluefile()
    {
      // ensure file descriptor is closed
      close();
    }

    void
    file_writer_bluefile::open(std::string fname){
      if (not d_initialized) {
        GR_LOG_DEBUG(d_logger,boost::format("Opening bluefile %s") % d_filename);

        // generate new object
        d_writer = new bluefile::BlueFile();

        // open file
        double nopen = d_writer->open(fname,bluefile::BlueFile::WRITE);
        d_initialized = true;

        // set file format
        if (d_data_type == "complex") {
          d_writer->set_format("CF");
          d_stype = 'F';
        }
        else if (d_data_type == "complex_int") {
          d_writer->set_format("CI");
          d_stype = 'I';
        }
        else if (d_data_type == "float") {
          d_writer->set_format("SF");
          d_stype = 'F';
        }
        else if (d_data_type == "int") {
          // TODO: Should we check what size integer this system really uses?
          // L == 32 bit integer
          d_writer->set_format("SL");
          d_stype = 'L';
        }
        else if (d_data_type == "short") {
          d_writer->set_format("SI");
          d_stype = 'I';
        }
        else if (d_data_type == "byte") {
          d_writer->set_format("SB");
          d_stype = 'B';
        }
        else {
          throw std::runtime_error("BlueFile Writer: Unsupported data type");
        }

        // set file start time and sampling rate
        d_writer->set_xdelta(1.0 / (double)d_rate);
        d_writer->set_xstart(d_samp_time.dtime());


        d_N = 0;
      }
    } /* end open() */

    void
    file_writer_bluefile::close(){
      if (d_initialized) {
        GR_LOG_DEBUG(d_logger,boost::format("Closing bluefile %s") % d_filename);

        // close the file
        d_writer->close();

        // cleanup
        delete d_writer;

        d_initialized = false;
      }
    }

    int
    file_writer_bluefile::write_impl(const void *in, int nitems){
      int nwritten = d_writer->write(in, d_stype, d_itemsize*nitems) / d_itemsize;

      // increment counter
      d_N += nwritten;

      return nwritten;
    }
  } /* namespace sandia_utils */
} /* namespace gr */
