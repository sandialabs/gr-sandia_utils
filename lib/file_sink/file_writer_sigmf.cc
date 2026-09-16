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

#include "file_writer_sigmf.h"
#include "../sigmf_metadata.h"

#include <iostream>
#include <stdio.h>
#include <boost/format.hpp>

namespace gr
{
  namespace sandia_utils
  {
    file_writer_sigmf::file_writer_sigmf( std::string data_type, std::string file_type, size_t itemsize,
        uint64_t nsamples, int rate, std::string out_dir, std::string name_spec, gr::logger_ptr logger ) :
        file_writer_base( data_type, file_type, itemsize, nsamples, rate, out_dir, name_spec, logger )
    {
      // fail at construction rather than when the first file is written
      d_datatype = sigmf_datatype( data_type );
    }

    file_writer_sigmf::~file_writer_sigmf()
    {
      // ensure file descriptor is closed
      close();
    }

    void file_writer_sigmf::open( std::string fname )
    {
      // the generated name is only a base - the samples and the metadata each
      // get the extension the specification requires
      std::string basename = sigmf_basename( fname );

      // report the data file as the file being written
      d_filename = basename + SIGMF_DATA_EXT;

      GR_LOG_DEBUG(d_logger,boost::format("Opening file %s") % d_filename.c_str());
      d_outfile.open( d_filename.c_str(), std::ofstream::binary );

      write_meta( basename + SIGMF_META_EXT );
    }

    void file_writer_sigmf::close()
    {
      if( d_outfile.is_open() )
      {
        GR_LOG_DEBUG(d_logger,boost::format("Closing file %s") % d_filename);
        d_outfile.flush();
        d_outfile.close();
      }
    }

    int file_writer_sigmf::write_impl( const void *in, int nitems )
    {
      d_outfile.write( (const char*)in, nitems * d_itemsize );
      return nitems;
    }

    /**
     * Write the companion metadata file
     *
     * @param fname - name of the .sigmf-meta file to write
     */
    void file_writer_sigmf::write_meta( std::string fname )
    {
      GR_LOG_DEBUG(d_logger,boost::format("Writing metadata %s") % fname.c_str());

      std::ofstream metafile( fname.c_str() );
      if( not metafile.is_open() )
      {
        GR_LOG_ERROR(d_logger,boost::format("Unable to write metadata file %s") % fname.c_str());
        return;
      }

      metafile << "{\n";
      metafile << "    \"global\": {\n";
      metafile << "        \"core:datatype\": \"" << d_datatype << "\",\n";
      metafile << "        \"core:sample_rate\": " << d_rate << ",\n";
      metafile << "        \"core:version\": \"" << SIGMF_VERSION << "\",\n";
      metafile << "        \"core:recorder\": \"gr-sandia_utils\"\n";
      metafile << "    },\n";
      metafile << "    \"captures\": [\n";
      metafile << "        {\n";
      metafile << "            \"core:sample_start\": 0,\n";
      metafile << "            \"core:frequency\": " << d_freq << ",\n";
      metafile << "            \"core:datetime\": \"" << sigmf_datetime_from_epoch( d_samp_time ) << "\"\n";
      metafile << "        }\n";
      metafile << "    ],\n";
      metafile << "    \"annotations\": []\n";
      metafile << "}\n";

      metafile.close();
    }

  } /* namespace sandia_utils */
} /* namespace gr */
