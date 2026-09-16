/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <gnuradio/sandia_utils/constants.h>

#include "file_reader_sigmf.h"
#include "../sigmf_metadata.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace gr
{
  namespace sandia_utils
  {

    /**
     * Opens a SigMF recording
     *
     * The filename may be either member of the .sigmf-data/.sigmf-meta pair or
     * the base name shared by the two.  Samples are read from the data file and
     * the tags are generated from the metadata file.
     *
     * @param filename - file to open
     */
    void file_reader_sigmf::open( const char *filename )
    {
      std::string basename = sigmf_basename( filename );

      // samples always come from the data file
      file_reader_base::open( (basename + SIGMF_DATA_EXT).c_str() );

      // metadata lives in a separate file alongside the samples
      if( d_is_open )
      {
        parse_metadata( basename + SIGMF_META_EXT );
      }

    } //end open

    /**
     * Reads the metadata file and generates the stream tags
     *
     * A missing or malformed metadata file is logged but is not fatal so that a
     * recording being written can still be played out.
     *
     * @param filename - name of the .sigmf-meta file to read
     */
    void file_reader_sigmf::parse_metadata( const std::string &filename )
    {
      pt::ptree meta;
      try
      {
        pt::read_json( filename, meta );
      }
      catch( const std::exception &e )
      {
        GR_LOG_ERROR( d_logger, boost::format( "Unable to read SigMF metadata file %s: %s" )
            % filename % e.what() );
        return;
      }

      // warn if the samples will be misinterpreted
      std::string datatype = meta.get<std::string>( "global.core:datatype", "" );
      size_t datatype_size = sigmf_datatype_size( datatype );
      if( datatype_size and (datatype_size != d_itemsize) )
      {
        GR_LOG_WARN( d_logger, boost::format(
            "SigMF datatype %s is %d bytes per sample but the file source is configured for %d" )
            % datatype % datatype_size % d_itemsize );
      }

      // only little endian samples are supported, and the size check above can
      // not catch a byte order mismatch
      if( datatype.size() > 3 and datatype.compare( datatype.size() - 3, 3, "_be" ) == 0 )
      {
        GR_LOG_WARN( d_logger, boost::format(
            "SigMF datatype %s is big endian and will not be interpreted correctly" )
            % datatype );
      }

      // sample rate is global to the recording.  Both keys are emitted: rate to
      // match the other file readers, and rx_rate for the UHD convention used
      // by the file sink and downstream blocks
      boost::optional<double> rate = meta.get_optional<double>( "global.core:sample_rate" );
      if( rate )
      {
        gr::tag_t tag;
        tag.value = pmt::from_double( *rate );

        tag.key = PMTCONSTSTR__rate();
        d_tags.push_back( tag );

        tag.key = PMTCONSTSTR__rx_rate();
        d_tags.push_back( tag );
      }

      // tags are applied at the first sample of the file, so only the first
      // capture segment can be represented
      pt::ptree captures = meta.get_child( "captures", pt::ptree() );
      if( captures.empty() )
      {
        GR_LOG_DEBUG( d_logger, boost::format( "No capture segments in %s" ) % filename );
        return;
      }
      if( captures.size() > 1 )
      {
        GR_LOG_WARN( d_logger, boost::format(
            "SigMF recording %s has %d capture segments; only the first is tagged" )
            % filename % captures.size() );
      }
      const pt::ptree &capture = captures.begin()->second;

      boost::optional<double> freq = capture.get_optional<double>( "core:frequency" );
      if( freq )
      {
        gr::tag_t tag;
        tag.key = PMTCONSTSTR__rx_freq();
        tag.value = pmt::from_double( *freq );
        d_tags.push_back( tag );
      }

      boost::optional<std::string> datetime = capture.get_optional<std::string>( "core:datetime" );
      if( datetime )
      {
        epoch_time file_time( 0.0 );
        if( sigmf_datetime_to_epoch( *datetime, file_time ) )
        {
          gr::tag_t tag;
          tag.key = PMTCONSTSTR__rx_time();
          tag.value = pmt::make_tuple( pmt::from_uint64( file_time.epoch_sec() ),
              pmt::from_double( file_time.epoch_frac() ) );
          d_tags.push_back( tag );
        }
        else
        {
          GR_LOG_ERROR( d_logger, boost::format( "Unable to parse SigMF datetime %s" )
              % *datetime );
        }
      }

    } //end parse_metadata

  }
// namespace sandia_utils
}// namespace gr
