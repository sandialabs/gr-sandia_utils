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

#include "file_writer_raw.h"

#include <iostream>
#include <stdio.h>
#include <boost/format.hpp>

namespace gr
{
  namespace sandia_utils
  {
    file_writer_raw::file_writer_raw( std::string data_type, std::string file_type, size_t itemsize, uint64_t nsamples,
        int rate, std::string out_dir, std::string name_spec, gr::logger_ptr logger ) :
        file_writer_base( data_type, file_type, itemsize, nsamples, rate, out_dir, name_spec, logger )
    {
      /* NOOP */
    }

    file_writer_raw::~file_writer_raw()
    {
      // ensure file descriptor is closed
      close();
    }

    void file_writer_raw::open( std::string fname )
    {
      GR_LOG_DEBUG(d_logger,boost::format("Opening file %s") % fname.c_str());
      d_outfile.open( fname.c_str(), std::ofstream::binary );
    }

    void file_writer_raw::close()
    {
      if( d_outfile.is_open() )
      {
        GR_LOG_DEBUG(d_logger,boost::format("Closing file %s") % d_filename);
        d_outfile.flush();
        d_outfile.close();
      }
    }

    int file_writer_raw::write_impl( const void *in, int nitems )
    {
      d_outfile.write( (const char*)in, nitems * d_itemsize );
      return nitems;
    }

  } /* namespace sandia_utils */
} /* namespace gr */
