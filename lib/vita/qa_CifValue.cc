/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <boost/test/unit_test.hpp>

#include "CifValue.h"


namespace gr
{
  namespace sandia_utils
  {
    class TestFixture
    {
      public:
        CifValue *dut;
        TestFixture()
        {
          dut = new CifValue();
        }
        virtual ~TestFixture()
        {
          delete dut;
        }
    };

    BOOST_FIXTURE_TEST_SUITE( qa_CifValue, TestFixture )


    BOOST_AUTO_TEST_CASE( test_instantiate1 )
    {
      BOOST_REQUIRE_EQUAL( 0, dut->getId() );
      BOOST_REQUIRE_EQUAL( (double)0, dut->getValue() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_instantiat3 )
    {
      CifValue dut2( 2, 15.3 );

      BOOST_REQUIRE_EQUAL( 2, dut2.getId() );
      BOOST_REQUIRE_EQUAL( (double)15.3, dut2.getValue() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_setId )
    {
      BOOST_REQUIRE_EQUAL( 0, dut->getId() );

      dut->setId( 10 );
      BOOST_REQUIRE_EQUAL( 10, dut->getId() );

      dut->setId( 200 );
      BOOST_REQUIRE_EQUAL( 200, dut->getId() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_setValue )
    {
      BOOST_REQUIRE_EQUAL( (double)0, dut->getValue() );

      dut->setValue( 10 );
      BOOST_REQUIRE_EQUAL( (double)10, dut->getValue() );

      dut->setValue( 200 );
      BOOST_REQUIRE_EQUAL( (double)200, dut->getValue() );

      return;
    } //end test_instantiate

    BOOST_AUTO_TEST_CASE( test_decode1 )
    {
      dut = new CifValue( 0, 28, 20, 0x000916f720000000 );

      BOOST_REQUIRE_EQUAL( 28, dut->getId() );
      BOOST_REQUIRE_EQUAL( (double)2440000000, dut->getValue() );

    } //end test_decode1

    BOOST_AUTO_TEST_SUITE_END()

  } /* namespace sandia_utils */
} /* namespace gr */


