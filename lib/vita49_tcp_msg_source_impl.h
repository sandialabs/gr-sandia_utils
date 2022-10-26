/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_VITA49_TCP_MSG_SOURCE_IMPL_H
#define INCLUDED_SANDIA_UTILS_VITA49_TCP_MSG_SOURCE_IMPL_H

#include <gnuradio/sandia_utils/vita49_tcp_msg_source.h>

#include"vita/vitarx.h"
#include"vita/ContextPacket.h"

namespace gr
{
  namespace sandia_utils
  {

    /**
     * Implmenting class of VITA 49 msg source
     */
    class vita49_tcp_msg_source_impl : public vita49_tcp_msg_source, public vita_rx_listener
    {
      private:
        vita_rx *rx;
        bool d_ignoreTime;
        bool d_ignoreTune;
        bool d_socketAlwaysOn;
        volatile bool d_running;

    public:
        /**
         * Constructor
         *
         * @param port - TCP port to listen on
         * @param socket_always_on - true to always leave TCP socket open, default false
         */
        vita49_tcp_msg_source_impl(int port, bool socket_always_on = false);

        /**
         * Deconstructor
         *
         */
        ~vita49_tcp_msg_source_impl();

        virtual bool start( void );

        virtual bool stop( void );

        /**
         * Called when a packet is received
         *
         * @param type - type of packet received
         * @param pkt - the packet received
         */
        virtual void received_packet( PacketType type, VRTPacket *pkt );

        /**
         * Sets Ignore Time behavior
         *
         * @param val - true, VITA time stamps are ignored, false, VITA time stamps are processed
         */
        virtual void setIgnoreTime( bool val = false );

        /**
         * Returns current Ignore Time behavior
         *
         * @return bool - true, VITA time stamps are ignored, false, VITA time stamps are processed
         */
        virtual bool getIgnoreTime( void );


        /**
         * Sets Ignore Tune behavior
         *
         * @param val - true, VITA context packets are ignored, false, VITA context packets are processed
         */
        virtual void setIgnoreTune( bool val = false );

        /**
         * Returns current Ignore Tune behavior
         *
         * @return bool - true, VITA context packets are ignored, false, VITA context packets are processed
         */
        virtual bool getIgnoreTune( void );

        /**
         * Ensures TCP socket is closed
         */
        virtual void closeSocket(void);

    private:
        /**
         * Handles VRT Data packets
         *
         * @param pkt - the packet to handle
         */
        void handle_data( VRTPacket *pkt );

        /**
         * Handles VRT Context packets
         *
         * @param pkt - the packet to handle
         */
        void handle_context( ContextPacket *pkt );


        void openBackend(void);
        void closeBackend(void);

    }; // end class vita49_tcp_msg_source_impl

  }// namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_VITA49_TCP_MSG_SOURCE_IMPL_H */

