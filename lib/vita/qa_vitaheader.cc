/* -*- c++ -*- */
/* 
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 * 
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <boost/test/unit_test.hpp>
#include "vitaheader.h"

namespace gr
{
  namespace sandia_utils
  {
    class TestFixture
    {
      public:
        vita_header *dut;
        TestFixture()
        {
          dut = new vita_header();
        }
        virtual ~TestFixture()
        {
          delete dut;
        }
    };

    BOOST_FIXTURE_TEST_SUITE( qa_vitaheader, TestFixture )



    BOOST_AUTO_TEST_CASE( test_instantiate )
    {

      BOOST_REQUIRE_EQUAL( PacketType::UNKNOWN, dut->getType() );
      BOOST_REQUIRE_EQUAL( false, dut->isC() );
      BOOST_REQUIRE_EQUAL( (uint8_t )0x00, dut->getIndicators() );
      BOOST_REQUIRE_EQUAL( TSI::NO_TSI, dut->getTsi() );
      BOOST_REQUIRE_EQUAL( TSF::NO_TSF, dut->getTsf() );
      BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getPktCount() );
      BOOST_REQUIRE_EQUAL( (uint16_t )0, dut->getPacketSize() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_decode1 )
    {
      uint32_t input = 0x4065000c;

      dut->unpack( input );

      BOOST_REQUIRE_EQUAL( PacketType::CONTEXT, dut->getType() );
      BOOST_REQUIRE_EQUAL( false, dut->isC() );
      BOOST_REQUIRE_EQUAL( (uint8_t )0x00, dut->getIndicators() );
      BOOST_REQUIRE_EQUAL( TSI::UTC, dut->getTsi() );
      BOOST_REQUIRE_EQUAL( TSF::REAL_TIME, dut->getTsf() );
      BOOST_REQUIRE_EQUAL( (uint8_t )5, dut->getPktCount() );
      BOOST_REQUIRE_EQUAL( (uint16_t )12, dut->getPacketSize() );

    } //end test_decode1

    BOOST_AUTO_TEST_CASE( test_decode2 )
    {
      uint32_t input = 0x1c688008;

      dut->unpack( input );

      BOOST_REQUIRE_EQUAL( PacketType::SIGNAL_DATA_ID, dut->getType() );
      BOOST_REQUIRE_EQUAL( true, dut->isC() );
      BOOST_REQUIRE_EQUAL( (uint8_t )0x04, dut->getIndicators() );
      BOOST_REQUIRE_EQUAL( TSI::UTC, dut->getTsi() );
      BOOST_REQUIRE_EQUAL( TSF::REAL_TIME, dut->getTsf() );
      BOOST_REQUIRE_EQUAL( (uint8_t )8, dut->getPktCount() );
      BOOST_REQUIRE_EQUAL( (uint16_t )0x8008, dut->getPacketSize() );

    } //end test_decode1


    BOOST_AUTO_TEST_CASE( test_copy )
    {
      vita_header h1;

      h1.unpack( 0x4065000c );
      dut->copy( h1 );

      BOOST_REQUIRE_EQUAL( PacketType::CONTEXT, dut->getType() );
      BOOST_REQUIRE_EQUAL( false, dut->isC() );
      BOOST_REQUIRE_EQUAL( (uint8_t )0x00, dut->getIndicators() );
      BOOST_REQUIRE_EQUAL( TSI::UTC, dut->getTsi() );
      BOOST_REQUIRE_EQUAL( TSF::REAL_TIME, dut->getTsf() );
      BOOST_REQUIRE_EQUAL( (uint8_t )5, dut->getPktCount() );
      BOOST_REQUIRE_EQUAL( (uint16_t )12, dut->getPacketSize() );

      return;
    }

    BOOST_AUTO_TEST_CASE( setType )
    {

      BOOST_REQUIRE_EQUAL( PacketType::UNKNOWN, dut->getType() );

      dut->setType( PacketType::COMMAND );
      BOOST_REQUIRE_EQUAL( PacketType::COMMAND, dut->getType() );

      dut->setType( PacketType::SIGNAL_DATA );
      BOOST_REQUIRE_EQUAL( PacketType::SIGNAL_DATA, dut->getType() );

      return;
    } //end setType

    BOOST_AUTO_TEST_SUITE_END()
  } /* namespace sandia_utils */
} /* namespace gr */

