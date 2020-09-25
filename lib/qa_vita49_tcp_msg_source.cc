/* -*- c++ -*- */
/* 
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 * 
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/top_block.h>
#include <gnuradio/attributes.h>
#include <gnuradio/blocks/message_debug.h>
#include <pmt/pmt.h>
#include <sandia_utils/vita49_tcp_msg_source.h>
#include <boost/test/unit_test.hpp>
#include "vita49_tcp_msg_source_impl.h"
#include "vita/VRTPacket.h"
#include "vita/ContextPacket.h"





namespace gr
{
  namespace sandia_utils
  {

    class TestFixture
    {
      public:

        gr::sandia_utils::vita49_tcp_msg_source::sptr dut;
        gr::blocks::message_debug::sptr debug_cmd;
        gr::blocks::message_debug::sptr debug_data;
        gr::top_block_sptr tb;

        TestFixture()
        {

          tb = gr::make_top_block("t2");

          dut = gr::sandia_utils::vita49_tcp_msg_source::make( 8107 );
          //dut_raw = new vita49_tcp_msg_source_impl( 8107 );
          //dut = gnuradio::get_initial_sptr( dut_raw );

          debug_cmd = gr::blocks::message_debug::make();
          debug_data = gr::blocks::message_debug::make();

          tb->msg_connect(dut, "tune", debug_cmd, "store");
          tb->msg_connect(dut, "out", debug_data, "store");

        }
        virtual ~TestFixture()
        {

        }
    };

    BOOST_FIXTURE_TEST_SUITE( qa_vita49_tcp_msg_source, TestFixture )

    BOOST_AUTO_TEST_CASE(test_instantiate)
    {
      tb->start();

      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));

      // stop
      tb->stop();
      tb->wait();
    }

    BOOST_AUTO_TEST_CASE( test_tune1 )
    {
      ContextPacket pkt;
      vita49_tcp_msg_source *raw;

      raw = dut.get();

      pkt.setTsEpoch( 50 );
      pkt.setTsFrac( 300000 );
      pkt.getHeader()->setType( PacketType::CONTEXT );
      pkt.getValues()->push_back( new CifValue( 29, 30720000 ) );
      pkt.getValues()->push_back( new CifValue( 27, 2440000000 ) );
      pkt.getValues()->push_back( new CifValue( 21, 30720000 ) );

      tb->start();


      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
      ((vita49_tcp_msg_source_impl*)raw)->received_packet( PacketType::CONTEXT, &pkt );

      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

      // stop
      tb->stop();
      tb->wait();

      // collect results
      BOOST_REQUIRE_EQUAL( 0, debug_data->num_messages() );
      BOOST_REQUIRE_EQUAL( 1, debug_cmd->num_messages() );

      pmt::pmt_t message = debug_cmd->get_message(0);
      pmt::pmt_t meta = pmt::car( message );

      printf("Generated Meta: \n%s\n", pmt::write_string(meta).c_str() );


      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("rate")) );
      BOOST_REQUIRE_CLOSE( 30720000, pmt::to_double( pmt::dict_ref(meta, pmt::intern("rate"), pmt::PMT_NIL) ), 1 );

      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("lo_freq")) );
      BOOST_REQUIRE_CLOSE( 2440000000, pmt::to_double( pmt::dict_ref(meta, pmt::intern("lo_freq"), pmt::PMT_NIL) ), 1 );

      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("bandwidth")) );
      BOOST_REQUIRE_CLOSE( 30720000, pmt::to_double( pmt::dict_ref(meta, pmt::intern("rate"), pmt::PMT_NIL) ), 1 );

      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("direction")) );
      BOOST_REQUIRE_EQUAL( pmt::intern("TX"), pmt::dict_ref(meta, pmt::intern("direction"), pmt::PMT_NIL) );

      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("time") ) );
      pmt::pmt_t pmt_time = pmt::dict_ref(meta, pmt::intern("time"), pmt::PMT_NIL);
      BOOST_REQUIRE_EQUAL( true, pmt::is_tuple( pmt_time ) );
      BOOST_REQUIRE_EQUAL( (uint64_t)50, pmt::to_uint64(pmt::tuple_ref(pmt_time, 0)) );
      BOOST_REQUIRE_CLOSE( 0.0000003, pmt::to_double(pmt::tuple_ref(pmt_time, 1)), 1 );

    }

    /**
     * This test is specifically ensuring the endian-ness of s16 data & IQ order is correct.
     * IF this test fails... data formats need to be deep dived
     */
    BOOST_AUTO_TEST_CASE( test_data1 )
    {
      VRTPacket pkt;
      vita49_tcp_msg_source *raw;
      printf("test_data1\n");

      raw = dut.get();

      pkt.setTsEpoch( 50 );
      pkt.setTsFrac( 300000 );
      pkt.getHeader()->setType( PacketType::SIGNAL_DATA );
      pkt.getPayload()->push_back( 0x01020304 );

      tb->start();


      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
      ((vita49_tcp_msg_source_impl*)raw)->received_packet( PacketType::SIGNAL_DATA, &pkt );

      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

      // stop
      tb->stop();
      tb->wait();

      // collect results
      BOOST_REQUIRE_EQUAL( 1, debug_data->num_messages() );
      BOOST_REQUIRE_EQUAL( 0, debug_cmd->num_messages() );

      pmt::pmt_t message = debug_data->get_message(0);
      pmt::pmt_t meta = pmt::car( message );
      pmt::pmt_t v_data = pmt::cdr( message );
      //grab data from the PMT
      std::vector<int16_t> data = pmt::s16vector_elements( v_data );

      BOOST_REQUIRE_EQUAL( 2, data.size() );
      BOOST_REQUIRE_EQUAL( (int16_t )0x0102, data.at(0) );
      BOOST_REQUIRE_EQUAL( (int16_t )0x0304, data.at(1) );


      printf("Generated Meta: \n%s\n", pmt::write_string(meta).c_str() );
      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("tx_time") ) );
      pmt::pmt_t pmt_time = pmt::dict_ref(meta, pmt::intern("tx_time"), pmt::PMT_NIL);
      BOOST_REQUIRE_EQUAL( true, pmt::is_tuple( pmt_time ) );
      BOOST_REQUIRE_EQUAL( (uint64_t)50, pmt::to_uint64(pmt::tuple_ref(pmt_time, 0)) );
      BOOST_REQUIRE_CLOSE( 0.0000003, pmt::to_double(pmt::tuple_ref(pmt_time, 1)), 1 );

      return;
    } //end test_data1



    BOOST_AUTO_TEST_CASE( test_tune2_notime )
    {
      ContextPacket pkt;
      vita49_tcp_msg_source *raw;

      dut->setIgnoreTime( true );
      raw = dut.get();


      pkt.setTsEpoch( 50 );
      pkt.setTsFrac( 300000 );
      pkt.getHeader()->setType( PacketType::CONTEXT );
      pkt.getValues()->push_back( new CifValue( 29, 30720000 ) );
      pkt.getValues()->push_back( new CifValue( 27, 2440000000 ) );
      pkt.getValues()->push_back( new CifValue( 21, 30720000 ) );

      tb->start();


      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
      ((vita49_tcp_msg_source_impl*)raw)->received_packet( PacketType::CONTEXT, &pkt );

      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

      // stop
      tb->stop();
      tb->wait();

      // collect results
      BOOST_REQUIRE_EQUAL( 0, debug_data->num_messages() );
      BOOST_REQUIRE_EQUAL( 1, debug_cmd->num_messages() );

      pmt::pmt_t message = debug_cmd->get_message(0);
      pmt::pmt_t meta = pmt::car( message );

      printf("Generated Meta: \n%s\n", pmt::write_string(meta).c_str() );


      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("rate")) );
      BOOST_REQUIRE_CLOSE( 30720000, pmt::to_double( pmt::dict_ref(meta, pmt::intern("rate"), pmt::PMT_NIL) ), 1 );

      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("lo_freq")) );
      BOOST_REQUIRE_CLOSE( 2440000000, pmt::to_double( pmt::dict_ref(meta, pmt::intern("lo_freq"), pmt::PMT_NIL) ), 1 );

      BOOST_REQUIRE_EQUAL( true, pmt::dict_has_key( meta, pmt::intern("bandwidth")) );
      BOOST_REQUIRE_CLOSE( 30720000, pmt::to_double( pmt::dict_ref(meta, pmt::intern("rate"), pmt::PMT_NIL) ), 1 );

      BOOST_REQUIRE_EQUAL( false, pmt::dict_has_key( meta, pmt::intern("direction")) );
      BOOST_REQUIRE_EQUAL( false, pmt::dict_has_key( meta, pmt::intern("time") ) );

    }

    /**
     * This test is specifically ensuring the endian-ness of s16 data & IQ order is correct.
     * IF this test fails... data formats need to be deep dived
     */
    BOOST_AUTO_TEST_CASE( test_data2_notime )
    {
      VRTPacket pkt;
      vita49_tcp_msg_source *raw;
      printf("test_data1\n");

      dut->setIgnoreTime( true );
      raw = dut.get();

      pkt.setTsEpoch( 50 );
      pkt.setTsFrac( 300000 );
      pkt.getHeader()->setType( PacketType::SIGNAL_DATA );
      pkt.getPayload()->push_back( 0x01020304 );

      tb->start();


      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
      ((vita49_tcp_msg_source_impl*)raw)->received_packet( PacketType::SIGNAL_DATA, &pkt );

      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

      // stop
      tb->stop();
      tb->wait();

      // collect results
      BOOST_REQUIRE_EQUAL( 1, debug_data->num_messages() );
      BOOST_REQUIRE_EQUAL( 0, debug_cmd->num_messages() );

      pmt::pmt_t message = debug_data->get_message(0);
      pmt::pmt_t meta = pmt::car( message );
      pmt::pmt_t v_data = pmt::cdr( message );
      //grab data from the PMT
      std::vector<int16_t> data = pmt::s16vector_elements( v_data );

      BOOST_REQUIRE_EQUAL( 2, data.size() );
      BOOST_REQUIRE_EQUAL( (int16_t )0x0102, data.at(0) );
      BOOST_REQUIRE_EQUAL( (int16_t )0x0304, data.at(1) );


      printf("Generated Meta: \n%s\n", pmt::write_string(meta).c_str() );
      BOOST_REQUIRE_EQUAL( false, pmt::dict_has_key( meta, pmt::intern("tx_time") ) );

      return;
    } //end test_data1


    BOOST_AUTO_TEST_CASE( test_tune2_notune )
    {
      ContextPacket pkt;
      vita49_tcp_msg_source *raw;

      dut->setIgnoreTune( true );
      raw = dut.get();


      pkt.setTsEpoch( 50 );
      pkt.setTsFrac( 300000 );
      pkt.getHeader()->setType( PacketType::CONTEXT );
      pkt.getValues()->push_back( new CifValue( 29, 30720000 ) );
      pkt.getValues()->push_back( new CifValue( 27, 2440000000 ) );
      pkt.getValues()->push_back( new CifValue( 21, 30720000 ) );

      tb->start();


      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
      ((vita49_tcp_msg_source_impl*)raw)->received_packet( PacketType::CONTEXT, &pkt );

      boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

      // stop
      tb->stop();
      tb->wait();

      // collect results
      BOOST_REQUIRE_EQUAL( 0, debug_data->num_messages() );
      BOOST_REQUIRE_EQUAL( 0, debug_cmd->num_messages() );

    }


    BOOST_AUTO_TEST_SUITE_END()
  } /* namespace sandia_utils */
} /* namespace gr */



