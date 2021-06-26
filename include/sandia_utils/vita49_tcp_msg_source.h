/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_VITA49_TCP_MSG_SOURCE_H
#define INCLUDED_SANDIA_UTILS_VITA49_TCP_MSG_SOURCE_H

#include <sandia_utils/api.h>
#include <gnuradio/block.h>

namespace gr
{
  namespace sandia_utils
  {

    /*!
     * \brief VITA 49 source block
     * \ingroup sandia_utils
     *
     * Assumes all VRT Signal Data packets are timed bursts
     *
     */
    class SANDIA_UTILS_API vita49_tcp_msg_source : virtual public gr::block
    {
      public:
        typedef std::shared_ptr<vita49_tcp_msg_source> sptr;

        /*!
         * \brief Return a shared_ptr to a new instance of sandia_utils::vita49_tcp_msg_source.
         *
         * @param port - TCP port to listen on
         */
        static sptr make( int port );


        /**
         * Sets Ignore Time behavior
         *
         * @param val - true, VITA time stamps are ignored, false, VITA time stamps are processed
         */
        virtual void setIgnoreTime( bool val = false ) = 0;

        /**
         * Returns current Ignore Time behavior
         *
         * @return bool - true, VITA time stamps are ignored, false, VITA time stamps are processed
         */
        virtual bool getIgnoreTime( void ) = 0;

        /**
         * Sets Ignore Tune behavior
         *
         * @param val - true, VITA context packets are ignored, false, VITA context packets are processed
         */
        virtual void setIgnoreTune( bool val = false ) = 0;

        /**
         * Returns current Ignore Tune behavior
         *
         * @return bool - true, VITA context packets are ignored, false, VITA context packets are processed
         */
        virtual bool getIgnoreTune( void ) = 0;
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_VITA49_TCP_MSG_SOURCE_H */

