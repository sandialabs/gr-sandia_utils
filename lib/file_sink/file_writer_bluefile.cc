/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
                                 std::string out_dir, std::string name_spec, gr::logger_ptr logger):
     file_writer_base( data_type, file_type, itemsize, nsamples, rate, out_dir, name_spec, logger )
    {
      // set to null
      d_writer = NULL;
      d_initialized = false;
      d_N = 0;
      d_stype = 0x00;
    }

    file_writer_bluefile::~file_writer_bluefile()
    {
      // ensure file descriptor is closed
      close();
    }

    void
    file_writer_bluefile::open(std::string fname){
      if (not d_initialized) {
        d_logger->debug("Opening bluefile {}", d_filename);

        // generate new object
        try
        {
          d_writer = new bluefile::BlueFile();
          double nopen = d_writer->open(fname,bluefile::BlueFile::WRITE);
        }
        catch (const std::runtime_error& error)
        {
          d_logger->warn("file sink bluefile runtime error on 'open': {}",error.what());
          return;
        }

        // open file
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

        // use utc start time
        // time_t current_time = time(0);
        // d_writer->set_xstart(double(current_time));
        d_writer->set_xstart(d_samp_time.dtime());

        // add frequency keyword
        std::string freq_keyword = "RFFREQ";
        std::string freq_value = std::to_string(d_freq);
        d_writer->add_keyword(freq_keyword,freq_value);



        d_N = 0;
      }
    } /* end open() */

    void file_writer_bluefile::close()
    {
      if( d_initialized )
      {
        d_logger->debug("Closing bluefile {}",d_filename);

        // close the file
        try
        {
          d_writer->flush();
          d_writer->close();
          // cleanup
          delete d_writer;
          d_writer = NULL;
        }
        catch (const std::runtime_error& error)
        {
          d_logger->debug("file sink bluefile flush and close runtime error: {}", error.what());
        }

        d_initialized = false;
      }
    }

    int file_writer_bluefile::write_impl( const void *in, int nitems )
    {
      if( !d_initialized )
      {
        d_logger->warn("file sink bluefile: uninitialized writer, dropping data");
        return nitems;
      }

      int nwritten = 0;
      try
      {
        nwritten = d_writer->write( in, d_stype, d_itemsize * nitems ) / d_itemsize;
      }
      catch (const std::runtime_error& error)
      {
        d_logger->warn("file sink bluefile write error: {}",error.what());
        nwritten = nitems;
      }

      // increment counter
      d_N += nwritten;

      return nwritten;
    }
  } /* namespace sandia_utils */
} /* namespace gr */
