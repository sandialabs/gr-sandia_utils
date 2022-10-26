/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <boost/test/unit_test.hpp>
#include "VRTPacket.h"


namespace gr
{
  namespace sandia_utils
  {

    class TestFixture
    {
      public:
        VRTPacket *dut;
        TestFixture()
        {
          dut = new VRTPacket();
        }
        virtual ~TestFixture()
        {
          delete dut;
        }
    };

    BOOST_FIXTURE_TEST_SUITE( qa_vrtpacket, TestFixture )



    BOOST_AUTO_TEST_CASE( test_instantiate )
    {

      BOOST_REQUIRE_EQUAL( PacketType::UNKNOWN, dut->getType() );
      BOOST_REQUIRE_EQUAL( (uint32_t )0, dut->getStreamId() );

      BOOST_REQUIRE_EQUAL( (uint8_t)0, dut->getClassId().getPadBitCount() );
      BOOST_REQUIRE_EQUAL( (uint32_t)0, dut->getClassId().getOui() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getClassId().getInfoClassCode() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getClassId().getPacketClassCode() );


      BOOST_REQUIRE_EQUAL( (uint32_t )0, dut->getTsEpoch() );
      BOOST_REQUIRE_EQUAL( (uint64_t )0, dut->getTsFrac() );
      BOOST_REQUIRE_EQUAL( 0, (int )dut->getPayload()->size() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_decode1 )
    {
      int stat;

      stat = dut->unpack( 0x4065000c );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000000 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x5ef41da6 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x000000e5 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x844fcdd5 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x28200000 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00001d4c );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000000 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x000916f7 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x20000000 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00001d4c );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000000 );
      BOOST_REQUIRE_EQUAL( 1, stat );


      BOOST_REQUIRE_EQUAL( PacketType::CONTEXT, dut->getType() );
      BOOST_REQUIRE_EQUAL( (uint32_t )0, dut->getStreamId() );
      BOOST_REQUIRE_EQUAL( false, dut->getHeader()->isC() );
      BOOST_REQUIRE_EQUAL( (uint8_t)0, dut->getClassId().getPadBitCount() );
      BOOST_REQUIRE_EQUAL( (uint32_t)0, dut->getClassId().getOui() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getClassId().getInfoClassCode() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getClassId().getPacketClassCode() );

      BOOST_REQUIRE_EQUAL( (uint32_t )0x5ef41da6, dut->getTsEpoch() );
      BOOST_REQUIRE_EQUAL( (uint64_t )0xe5844fcdd5, dut->getTsFrac() );
      BOOST_REQUIRE_EQUAL( 7, (int )dut->getPayload()->size() );

      BOOST_REQUIRE_EQUAL( (uint32_t )0x28200000, dut->getPayload()->at(0) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00001d4c, dut->getPayload()->at(1) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00000000, dut->getPayload()->at(2) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x000916f7, dut->getPayload()->at(3) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x20000000, dut->getPayload()->at(4) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00001d4c, dut->getPayload()->at(5) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00000000, dut->getPayload()->at(6) );

    } //end test_decode1

    BOOST_AUTO_TEST_CASE( test_decode2 )
    {
      int stat;

      printf("\t test_decode2()\n");
      stat = dut->unpack( 0x4065000c ); //header
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000000 ); //stream
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x5F3316BC ); //epoc
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000005 ); //frac
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0xEFEB1F00 ); //frac
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x28200000 ); //cif0
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00001d4c );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000000 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x000916f7 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x20000000 );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00001d4c );
      BOOST_REQUIRE_EQUAL( 2, stat );
      stat = dut->unpack( 0x00000000 );
      BOOST_REQUIRE_EQUAL( 1, stat );


      BOOST_REQUIRE_EQUAL( PacketType::CONTEXT, dut->getType() );
      BOOST_REQUIRE_EQUAL( (uint32_t )0, dut->getStreamId() );
      BOOST_REQUIRE_EQUAL( false, dut->getHeader()->isC() );
      BOOST_REQUIRE_EQUAL( (uint8_t)0, dut->getClassId().getPadBitCount() );
      BOOST_REQUIRE_EQUAL( (uint32_t)0, dut->getClassId().getOui() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getClassId().getInfoClassCode() );
      BOOST_REQUIRE_EQUAL( (uint16_t)0, dut->getClassId().getPacketClassCode() );

      BOOST_REQUIRE_EQUAL( (uint32_t )0x5F3316BC, dut->getTsEpoch() );
      BOOST_REQUIRE_EQUAL( (uint64_t )0x00000005EFEB1F00, dut->getTsFrac() );
      BOOST_REQUIRE_EQUAL( 7, (int )dut->getPayload()->size() );

      BOOST_REQUIRE_EQUAL( (uint32_t )0x28200000, dut->getPayload()->at(0) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00001d4c, dut->getPayload()->at(1) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00000000, dut->getPayload()->at(2) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x000916f7, dut->getPayload()->at(3) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x20000000, dut->getPayload()->at(4) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00001d4c, dut->getPayload()->at(5) );
      BOOST_REQUIRE_EQUAL( (uint32_t )0x00000000, dut->getPayload()->at(6) );

      dut->debug_print();

    } //end test_decode2


    BOOST_AUTO_TEST_SUITE_END()
  } /* namespace sandia_utils */
} /* namespace gr */

