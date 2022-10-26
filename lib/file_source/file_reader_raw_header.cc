/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <gnuradio/sandia_utils/constants.h>

#include "file_reader_raw_header.h"

namespace gr
{
  namespace sandia_utils
  {

    /**
     * Opens a raw header file
     *
     * @param filename - file to open
     */
    void file_reader_raw_header::open( const char *filename )
    {
      file_reader_base::open( filename );

      // read metadata information and populate tags
      if( d_is_open )
      {
        double metadata[3];
        if( fread( &metadata[0], sizeof(double), 3, d_fp ) != 3 )
        {
          throw std::runtime_error( "Unable to read metadata from file" );
        }

        // set timed
        epoch_time file_time( metadata[2] );

        gr::tag_t tag;
        tag.key = PMTCONSTSTR__rx_freq();
        tag.value = pmt::from_double( metadata[0] );
        d_tags.push_back( tag );
        tag.key = PMTCONSTSTR__rate();
        tag.value = pmt::from_double( metadata[1] );
        d_tags.push_back( tag );
        tag.key = PMTCONSTSTR__rx_time();
        tag.value = pmt::make_tuple( pmt::from_uint64( file_time.epoch_sec() ),
            pmt::from_double( file_time.epoch_frac() ) );
        d_tags.push_back( tag );
      }

    } //end open

  }
// namespace sandia_utils
}// namespace gr

