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

#include "vita49_tcp_msg_source_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/sandia_utils/constants.h>
#include <arpa/inet.h>
#include <endian.h>

namespace gr
{
  namespace sandia_utils
  {

  vita49_tcp_msg_source::sptr vita49_tcp_msg_source::make(int port, bool socket_always_on)
  {
      return gnuradio::get_initial_sptr(
          new vita49_tcp_msg_source_impl(port, socket_always_on));
    }

    /*
     * The private constructor
     */
    vita49_tcp_msg_source_impl::vita49_tcp_msg_source_impl(int port,
                                                           bool socket_always_on)
        : gr::block("vita49_tcp_msg_source",
                    gr::io_signature::make(0, 0, 0),
                    gr::io_signature::make(0, 0, 0))
    {
      //int stat;

      d_socketAlwaysOn = socket_always_on;
      d_running = false;
      d_ignoreTime = false;
      d_ignoreTune = false;
      rx = new vita_rx( port );
      rx->add_listener( this );

      if (d_socketAlwaysOn) {
          openBackend();
      }

      /**
      stat = rx->start();
      if( stat != 1 )
      {
        d_logger->warn("Error starting vita_rx component");
      }
      */

      message_port_register_out(PMTCONSTSTR__out());
      message_port_register_out(PMTCONSTSTR__tune());

      return;
    } //end constructor

    /*
     * Our virtual destructor.
     */
    vita49_tcp_msg_source_impl::~vita49_tcp_msg_source_impl()
    {
        closeSocket();

        return;
    } //end deconstructor


    bool vita49_tcp_msg_source_impl::start( void )
    {

      d_logger->info("vita49_tcp_msg_source_impl::start()");

      d_running = true;

      if (!d_socketAlwaysOn) {
          openBackend();
      }

      return true;
    }

    bool vita49_tcp_msg_source_impl::stop( void )
    {
        d_running = false;

        d_logger->info("vita49_tcp_msg_source_impl::stop()");

        if (!d_socketAlwaysOn) {
            closeBackend();
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
        if (d_running) {
            switch (type) {
            case PacketType::SIGNAL_DATA:
            case PacketType::SIGNAL_DATA_ID: {
                handle_data(pkt);
                break;
            }
            case PacketType::CONTEXT: {
                handle_context((ContextPacket*)pkt);
                break;
            }
            default: {
                // drop it
                d_logger->debug("Dropping unhandled packet type");
                break;
            }
            } // end switch
        } else {
            d_logger->debug("GR Block not running, Dropping packet");
        }

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
     * Ensures TCP socket is closed
     */
    void vita49_tcp_msg_source_impl::closeSocket(void)
    {
        closeBackend();

        return;
    } // end closeSocket

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
      for(size_t i = 0; i < pkt->getPayload()->size(); i++ )
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
          metadata = pmt::dict_add(metadata, PMTCONSTSTR__tx_time(), time_tag);
        }

        // ship it!
        message_port_pub(PMTCONSTSTR__out(), pmt::cons(metadata, data_vec));
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
        metadata = pmt::dict_add(metadata, PMTCONSTSTR__time(), time_tag);
        metadata = pmt::dict_add(metadata, PMTCONSTSTR__direction(), PMTCONSTSTR__TX());
      }

      for( CifValue *cf : *pkt->getValues() )
      {
        switch( cf->getId() )
        {
          case (29):
          {
            //bandwidth
            metadata = pmt::dict_add(
                metadata, PMTCONSTSTR__bandwidth(), pmt::from_double(cf->getValue()));
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
            metadata = pmt::dict_add(
                metadata, PMTCONSTSTR__lo_freq(), pmt::from_double(cf->getValue()));
            break;
          }
          case (21):
          {
            //Sample Rate
            metadata = pmt::dict_add(
                metadata, PMTCONSTSTR__rate(), pmt::from_double(cf->getValue()));
            break;
          }

        } //end switch

      } //end for( cf


      if( !d_ignoreTune )
      {
          message_port_pub(PMTCONSTSTR__tune(), pmt::cons(metadata, data_vec));
          // printf("VITA49 - Published Command PDU\n");
      }

      return;
    }

    void vita49_tcp_msg_source_impl::openBackend(void)
    {
        int stat;

        d_logger->info("openBackend");

        stat = rx->start();
        if (stat != 1) {
            d_logger->warn("Error starting vita_rx component");
        }

        return;
    }

    void vita49_tcp_msg_source_impl::closeBackend(void)
    {
        int stat;

        d_logger->info("closeBackend");

        stat = rx->stop();
        if (stat != 1) {
            d_logger->warn("Error stopping vita_rx component");
        }

        return;
    }

  } /* namespace sandia_utils */
} /* namespace gr */

