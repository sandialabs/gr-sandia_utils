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

#include "file_writer_raw_header.h"

#include <iostream>
#include <stdio.h>

namespace gr
{
  namespace sandia_utils
  {
    file_writer_raw_header::file_writer_raw_header( std::string data_type, std::string file_type, size_t itemsize,
        uint64_t nsamples, int rate, std::string out_dir, std::string name_spec, gr::logger_ptr logger ) :
            file_writer_base( data_type, file_type, itemsize, nsamples, rate, out_dir, name_spec, logger )
    {
      /* NOOP */
    }

    file_writer_raw_header::~file_writer_raw_header()
    {
      // ensure file descriptor is closed
      close();
    }

    void file_writer_raw_header::open( std::string fname )
    {
      d_logger->debug("Opening file {}",fname.c_str());
      d_outfile.open( fname.c_str(), std::ofstream::binary );

      // write header
      // format is: (frequency, rate, sample_time)
      double freq = (double)d_freq;
      double rate = (double)d_rate;
      double start_time = d_samp_time.dtime();
      d_outfile.write( (const char*)&freq, sizeof(double) );
      d_outfile.write( (const char*)&rate, sizeof(double) );
      d_outfile.write( (const char*)&start_time, sizeof(double) );
    }

    void file_writer_raw_header::close()
    {
      if( d_outfile.is_open() )
      {
        d_logger->debug("Closing file {}",d_filename);
        d_outfile.flush();
        d_outfile.close();
      }
    }

    int file_writer_raw_header::write_impl( const void *in, int nitems )
    {
      d_outfile.write( (const char*)in, nitems * d_itemsize );
      return nitems;
    }

  } /* namespace sandia_utils */
} /* namespace gr */
