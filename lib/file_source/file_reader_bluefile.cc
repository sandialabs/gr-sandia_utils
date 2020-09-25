/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include<sandia_utils/constants.h>
#include "file_reader_bluefile.h"

namespace gr
{
  namespace sandia_utils
  {

    void file_reader_bluefile::open( const char *filename )
    {
      if( d_is_open )
      {
        this->close();
      }

      GR_LOG_DEBUG( d_logger, boost::format("File Reader: Opening file %s") % filename );
      d_filename = std::string( filename );

      d_blue_reader = new bluefile::BlueFile();

      // open file
      d_file_size = int( d_blue_reader->open( filename, bluefile::BlueFile::READ ) );
      d_type = d_blue_reader->get_format();
      d_bpe = d_blue_reader->get_bpe();
      d_bps = d_blue_reader->get_bps();

      // read header information and populate tags - no frequency information right now
      double rate = 1.0 / d_blue_reader->get_xdelta();
      epoch_time file_time( d_blue_reader->get_xstart() );

      gr::tag_t tag;
      d_tags.clear();
      // tag.key = FREQ_KEY; tag.value = pmt::from_double(metadata[0]); d_tags.push_back(tag);
      tag.key = RATE_KEY;
      tag.value = pmt::from_double( rate );
      d_tags.push_back( tag );
      tag.key = RX_TIME_KEY;
      tag.value = pmt::make_tuple( pmt::from_uint64( file_time.epoch_sec() ),
          pmt::from_double( file_time.epoch_frac() ) );
      d_tags.push_back( tag );

      // check for frequency tag
      try
      {
        char frequency[50];
        double freq = 0.0;
        d_blue_reader->get_keyword( "RFFREQ", frequency, 50 );
        try
        {
          freq = std::stod( frequency, NULL );
          tag.key = FREQ_KEY;
          tag.value = pmt::from_double( freq );
          d_tags.push_back( tag );
        }
        catch ( ... )
        {
        }
      }
      catch ( ... )
      {
      }

      // ensure beginning of file
      d_blue_reader->seek( 0, bluefile::BlueFile::SET );

      // all done
      d_is_open = true;
    }

    int file_reader_bluefile::read( char *dest, int nitems )
    {
      if( d_blue_reader->is_open() )
      {
        // return d_blue_reader->read(dest,d_bpe,nitems);
        return d_blue_reader->read( dest, d_bps, nitems );
      }

      return 0;
    }

    bool file_reader_bluefile::seek( long seek_point, int whence )
    {
      if( d_is_open and d_blue_reader->is_open() )
      {
        GR_LOG_DEBUG( d_logger, boost::format("Seeking in bluefile reader: %ld, %d") % seek_point % whence );
        d_blue_reader->seek( double( seek_point ), bluefile::BlueFile::BlueFileSeekEnum( whence ) );
      }
      return true;
    }

    void file_reader_bluefile::close()
    {
      // protect against calling close before open
      if( not d_is_open )
      {
        return;
      }

      if( d_blue_reader->is_open() )
      {
        GR_LOG_DEBUG( d_logger, boost::format("File Reader: Closing file %s") % d_filename );
        d_blue_reader->close();

        // set flag
        d_is_open = false;

        // clean up
        delete d_blue_reader;
        d_blue_reader = NULL;
      }
    }

    bool file_reader_bluefile::eof()
    {
      if( not d_blue_reader->is_open() )
      {
        return true;
      }

      return (int( d_blue_reader->tell() ) == d_file_size);
    }

  }
// namespace sandia_utils
}// namespace gr

