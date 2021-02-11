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

#include <endian.h>
#include <arpa/inet.h>
#include <gnuradio/io_signature.h>
#include "vita49_tcp_msg_source_impl.h"
#include <sandia_utils/constants.h>

namespace gr
{
  namespace sandia_utils
  {

    vita49_tcp_msg_source::sptr vita49_tcp_msg_source::make( int port )
    {
      return gnuradio::get_initial_sptr( new vita49_tcp_msg_source_impl( port ) );
    }

    /*
     * The private constructor
     */
    vita49_tcp_msg_source_impl::vita49_tcp_msg_source_impl( int port ) : gr::block( "vita49_tcp_msg_source",
        gr::io_signature::make( 0, 0, 0 ), gr::io_signature::make( 0, 0, 0 ) )
    {
      int stat;

      d_ignoreTime = false;
      d_ignoreTune = false;
      rx = new vita_rx( port );
      rx->add_listener( this );

      /**
      stat = rx->start();
      if( stat != 1 )
      {
        GR_LOG_WARN(d_logger, "Error starting vita_rx component");
      }
      */

      message_port_register_out( OUT_KEY );
      message_port_register_out( TUNE_KEY );

      return;
    } //end constructor

    /*
     * Our virtual destructor.
     */
    vita49_tcp_msg_source_impl::~vita49_tcp_msg_source_impl()
    {
      rx->stop();

      return;
    } //end deconstructor


    bool vita49_tcp_msg_source_impl::start( void )
    {
      int stat;

      GR_LOG_INFO( d_logger, "vita49_tcp_msg_source_impl::start()");

      stat = rx->start();
      if( stat != 1 )
      {
        GR_LOG_WARN(d_logger, "Error starting vita_rx component");
      }

      return true;
    }

    bool vita49_tcp_msg_source_impl::stop( void )
    {
      int stat;

      GR_LOG_INFO( d_logger, "vita49_tcp_msg_source_impl::stop()");

      stat = rx->stop();
      if( stat != 1 )
      {
        GR_LOG_WARN(d_logger, "Error stopping vita_rx component");
      }

      return true;
    } // end stop

    /**
     * Called when a packet is received
     *
     * @param type - type of packet received
     * @param pkt - the packet received
     */
    void vita49_tcp_msg_source_impl::received_packet( PacketType type, VRTPacket *pkt )
    {
      switch ( type )
      {
        case PacketType::SIGNAL_DATA:
        case PacketType::SIGNAL_DATA_ID:
        {
          handle_data( pkt );
          break;
        }
        case PacketType::CONTEXT:
        {
          handle_context( (ContextPacket*)pkt );
          break;
        }
        default:
        {
          //drop it
          GR_LOG_DEBUG( d_logger, "Dropping unhandled packet type");
          break;
        }
      } //end switch

      return;
    } //end received_packet

    /**
     * Sets Ignore Time behavior
     *
     * @param val - true, VITA time stamps are ignored, false, VITA time stamps are processed
     */
    void vita49_tcp_msg_source_impl::setIgnoreTime( bool val )
    {
      d_ignoreTime = val;
      return;
    }

    /**
     * Returns current Ignore Time behavior
     *
     * @return bool - true, VITA time stamps are ignored, false, VITA time stamps are processed
     */
    bool vita49_tcp_msg_source_impl::getIgnoreTime( void )
    {
      return d_ignoreTime;
    }

    /**
     * Sets Ignore Tune behavior
     *
     * @param val - true, VITA context packets are ignored, false, VITA context packets are processed
     */
    void vita49_tcp_msg_source_impl::setIgnoreTune( bool val )
    {
      d_ignoreTune = val;
      return;
    }

    /**
     * Returns current Ignore Tune behavior
     *
     * @return bool - true, VITA context packets are ignored, false, VITA context packets are processed
     */
    bool vita49_tcp_msg_source_impl::getIgnoreTune( void )
    {
      return d_ignoreTune;
    }

    /**
     * Handles VRT Data packets
     *
     * @param pkt - the packet to handle
     */
    void vita49_tcp_msg_source_impl::handle_data( VRTPacket *pkt )
    {
      pmt::pmt_t metadata;
      pmt::pmt_t data_vec;


      //VRT packet payload is 32bit word oriented. Each 32bit word is a single sample
      //  16bit for I and 16bit for Q. Thus vector size of int16_t is 2x payload side
      std::vector<int16_t> dt;
      int16_t iv, qv;
      uint32_t raw;

      //printf("VRT had %d word\n", pkt->getPayload()->size() );
      for(int i = 0; i < pkt->getPayload()->size(); i++ )
      {
        raw = pkt->getPayload()->at( i );
        //raw = be32toh( raw );

        iv = ((raw & 0xffff0000) >> 16) & 0x0000ffff;
        qv = raw & 0x0000ffff;

        //iv = ntohs( iv );
        //qv = ntohs( qv );

        dt.push_back(iv);
        dt.push_back(qv);

      } //end for(i

      //printf("DT has %d elements\n", dt.size() );
      if( dt.size() > 0 )
      {
        metadata = pmt::make_dict();
        data_vec = pmt::init_s16vector( dt.size(), dt.data() );

        if( !d_ignoreTime )
        {
          pmt::pmt_t time_tag = pmt::make_tuple(pmt::from_uint64( pkt->getTsEpoch() ), pmt::from_double( pkt->getTsFrac() * 1e-12));
          metadata = pmt::dict_add( metadata, TX_TIME_KEY, time_tag );
        }

        // ship it!
        message_port_pub( OUT_KEY, pmt::cons( metadata, data_vec ) );
        //printf("VITA49 - Published PDU\n");
      }

      return;
    } //end handle_data

    /**
     * Handles VRT Context packets
     *
     * @param pkt - the packet to handle
     */
    void vita49_tcp_msg_source_impl::handle_context( ContextPacket *pkt )
    {
      pmt::pmt_t metadata = pmt::make_dict();
      pmt::pmt_t data_vec = pmt::make_s16vector(1, 0);

      if( !d_ignoreTime )
      {
        pmt::pmt_t time_tag = pmt::make_tuple(pmt::from_uint64( pkt->getTsEpoch() ), pmt::from_double( pkt->getTsFrac() * 1e-12));
        metadata = pmt::dict_add( metadata, TIME_KEY, time_tag );
        metadata = pmt::dict_add( metadata, CMD_DIRECTION, CMD_TX );
      }

      for( CifValue *cf : *pkt->getValues() )
      {
        switch( cf->getId() )
        {
          case (29):
          {
            //bandwidth
            metadata = pmt::dict_add( metadata, CMD_BW_KEY, pmt::from_double(cf->getValue()) );
            break;
          }
          case (28):
          {
            //IF Ref
            break;
          }
          case (27):
          {
            //RF Ref
            metadata = pmt::dict_add( metadata, CMD_LO_FREQ_KEY, pmt::from_double(cf->getValue()) );
            break;
          }
          case (21):
          {
            //Sample Rate
            metadata = pmt::dict_add( metadata, CMD_RATE_KEY, pmt::from_double(cf->getValue()) );
            break;
          }

        } //end switch

      } //end for( cf


      if( !d_ignoreTune )
      {
        message_port_pub( TUNE_KEY, pmt::cons( metadata, data_vec ) );
        //printf("VITA49 - Published Command PDU\n");
      }

      return;
    }

  } /* namespace sandia_utils */
} /* namespace gr */

