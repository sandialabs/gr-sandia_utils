/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <boost/test/unit_test.hpp>
#include "vitatrailer.h"

namespace gr
{
  namespace sandia_utils
  {

    class TestFixture
    {
      public:
        vita_trailer *dut;
        TestFixture()
        {
          dut = new vita_trailer();
        }
        virtual ~TestFixture()
        {
          delete dut;
        }
    };

    BOOST_FIXTURE_TEST_SUITE( qa_vitatrailer, TestFixture )


    BOOST_AUTO_TEST_CASE( test_instantiate )
    {
      printf( "\test_instantiate()\n" );

      for( int i = 0; i < 12; i++ )
      {
        BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getEnable( i ) );
        BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getIndicators( i ) );
      }

      BOOST_REQUIRE_EQUAL( false, dut->isE() );
      BOOST_REQUIRE_EQUAL( 0, dut->getContextCount() );
      BOOST_REQUIRE_EQUAL( SF_NONE, dut->getSampleFrameIndicator() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_decode1 )
    {
      uint32_t input = 0x40040000;
      printf( "\test_decode1()\n" );

      dut->unpack( input );

      for( int i = 0; i < 12; i++ )
      {
        //printf("Debug: Idx %d E %u I %u \n", i, dut->getEnable(i), dut->getIndicators(i) );
        if( i == 1 )
        {
          BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( i ) );
          BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( i ) );
        }
        else
        {
          BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getEnable( i ) );
          BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getIndicators( i ) );
        }

      } //end for(i

      BOOST_REQUIRE_EQUAL( false, dut->isE() );
      BOOST_REQUIRE_EQUAL( 0, dut->getContextCount() );
      BOOST_REQUIRE_EQUAL( SF_NONE, dut->getSampleFrameIndicator() );

    } //end test_decode1

    BOOST_AUTO_TEST_CASE( test_decode_e )
    {
      uint32_t input = 0x00000000;
      printf( "\test_decode_e()\n" );

      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( false, dut->isE() );

      input = 0x00000080;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( true, dut->isE() );

      input = 0x00000080 ^ 0xffffffff;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( false, dut->isE() );

      return;
    }

    BOOST_AUTO_TEST_CASE( test_decode_context )
    {
      uint32_t input = 0x00000000;
      printf( "\test_decode_context()\n" );

      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( 0, dut->getContextCount() );

      input = 0xffffffff;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( 0x7f, dut->getContextCount() );

      input = 0x00000004;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( 0x04, dut->getContextCount() );

      return;
    }

    BOOST_AUTO_TEST_CASE( test_decode_enable )
    {
      uint32_t input = 0x80000000;

      printf( "\test_decode_enable()\n" );

      for( int i = 0; i < 12; i++ )
      {
        input = 0x80000000 >> i;
        dut->unpack( input );
        BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( i ) );
      } //end for(i

      return;
    } //end test_decode_enable

    BOOST_AUTO_TEST_CASE( test_decode_indicators )
    {
      uint32_t input;

      printf( "\test_decode_indicators()\n" );

      for( int i = 0; i < 12; i++ )
      {
        input = 0x00080000 >> i;
        dut->unpack( input );
        BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( i ) );
      } //end for(i

      return;
    } //test_decode_indicators

    BOOST_AUTO_TEST_CASE( test_copy )
    {
      vita_trailer h1;
      uint32_t input = 0x40040000;

      printf( "\test_copy()\n" );

      h1.unpack( input );
      dut->copy( h1 );

      for( int i = 0; i < 12; i++ )
      {
        //printf("Debug: Idx %d E %u I %u \n", i, dut->getEnable(i), dut->getIndicators(i) );
        if( i == 1 )
        {
          BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( i ) );
          BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( i ) );
        }
        else
        {
          BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getEnable( i ) );
          BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getIndicators( i ) );
        }

      } //end for(i

      BOOST_REQUIRE_EQUAL( false, dut->isE() );
      BOOST_REQUIRE_EQUAL( 0, dut->getContextCount() );
      BOOST_REQUIRE_EQUAL( SF_NONE, dut->getSampleFrameIndicator() );

      return;
    }

    BOOST_AUTO_TEST_CASE( test_sampleframe )
    {
      uint32_t input = 0x00c00c00;
      printf( "\test_sampleframe()\n" );

      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( 9 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( 9 ) );
      BOOST_REQUIRE_EQUAL( SF_END, dut->getSampleFrameIndicator() );

      input = 0x00c00400;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getIndicators( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( 9 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( 9 ) );
      BOOST_REQUIRE_EQUAL( SF_FIRST, dut->getSampleFrameIndicator() );

      input = 0x00c00800;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getEnable( 9 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getIndicators( 9 ) );
      BOOST_REQUIRE_EQUAL( SF_MID, dut->getSampleFrameIndicator() );

      input = 0x00000c00;
      dut->unpack( input );
      BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getEnable( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( 8 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )0, dut->getEnable( 9 ) );
      BOOST_REQUIRE_EQUAL( (uint8_t )1, dut->getIndicators( 9 ) );
      BOOST_REQUIRE_EQUAL( SF_NONE, dut->getSampleFrameIndicator() );

      return;
    }

    BOOST_AUTO_TEST_SUITE_END()

  } /* namespace sandia_utils */
} /* namespace gr */

