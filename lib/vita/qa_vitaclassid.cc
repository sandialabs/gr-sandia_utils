/* -*- c++ -*- */
/* 
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 * 
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <boost/test/unit_test.hpp>
#include "vitaclassid.h"


namespace gr
{
  namespace sandia_utils
  {


    class TestFixture
    {
      public:
        vita_class_id *dut;
        TestFixture()
        {
          dut = new vita_class_id();
        }
        virtual ~TestFixture()
        {
          delete dut;
        }
    };

    BOOST_FIXTURE_TEST_SUITE( qa_vitaclassid, TestFixture )



    BOOST_AUTO_TEST_CASE( test_instantiate )
    {
      printf( "\test_instantiate()\n" );

      BOOST_REQUIRE_EQUAL( (uint8_t)0, dut->getPadBitCount() );
      BOOST_REQUIRE_EQUAL( (uint32_t)0, dut->getOui() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getInfoClassCode() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getPacketClassCode() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_decode1 )
    {
      uint32_t input = 0x00000000;
      int stat;

      printf( "\test_decode1()\n" );

      stat = dut->unpack( input );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( input );
      BOOST_REQUIRE_EQUAL( 1, stat );
      stat = dut->unpack( input );
      BOOST_REQUIRE_EQUAL( 0, stat );


    } //end test_decode1

    BOOST_AUTO_TEST_CASE( test_decode2 )
    {
      vita_class_id h1;
      uint32_t input = 0x40040000;

      printf( "\test_decode2()\n" );

      dut->unpack( input );
      dut->unpack( input );

      BOOST_REQUIRE_EQUAL( (uint8_t)0x08, dut->getPadBitCount() );
      BOOST_REQUIRE_EQUAL( (uint32_t)0x00040000, dut->getOui() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0x4004, dut->getInfoClassCode() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0x0000, dut->getPacketClassCode() );


      return;
    }

    BOOST_AUTO_TEST_CASE( test_copy )
    {
      vita_class_id h1;
      uint32_t input = 0x40040000;

      printf( "\test_copy()\n" );

      h1.unpack( input );
      h1.unpack( input );
      dut->copy( h1 );

      BOOST_REQUIRE_EQUAL( (uint8_t)0x08, dut->getPadBitCount() );
      BOOST_REQUIRE_EQUAL( (uint32_t)0x00040000, dut->getOui() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0x4004, dut->getInfoClassCode() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0x0000, dut->getPacketClassCode() );


      return;
    }


    BOOST_AUTO_TEST_SUITE_END()

  } /* namespace sandia_utils */
} /* namespace gr */

