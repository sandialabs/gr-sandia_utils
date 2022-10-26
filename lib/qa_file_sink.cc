/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "gnuradio/sandia_utils/constants.h"
#include "gnuradio/sandia_utils/file_sink.h"
#include "gnuradio/sandia_utils/file_source.h"
#include <gnuradio/block_detail.h>
#include <gnuradio/blocks/message_debug.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/tag_debug.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/buffer.h>
#include <gnuradio/tags.h>
#include <gnuradio/top_block.h>
#include <pmt/pmt.h>
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>


namespace gr {
namespace sandia_utils {
bool remove_file(std::string fname)
{
    boost::filesystem::path fs(fname);
    if (boost::filesystem::exists(fs)) {
        return boost::filesystem::remove(fs);
    }

    return false;
}

class streaming_interface
{
    // simulate a streaming interface by allowing for an exact number
    // of samples and tags to be "pushed" to a block through the
    // work function
public:
    streaming_interface(gr::sandia_utils::file_sink::sptr sink) : d_sink(sink)
    {

        // instantiate blocks
        d_source = gr::sandia_utils::file_source::make(
            sizeof(gr_complex) * 1, "", "raw", false, false);
        d_source->set_begin_tag(pmt::PMT_NIL);
        d_source->add_file_tags(false);
        d_source->set_file_queue_depth(1);
        d_debug = gr::blocks::message_debug::make();

        // create top block
        // start and stop immediately so all buffers can be initialized properly
        // but the work function is not called when the input buffer is updated
        // Note: There may be a better way to do this
        d_tb = gr::make_top_block("test");
        d_tb->connect(d_source, 0, d_sink, 0);
        d_tb->msg_connect(d_sink, "pdu", d_debug, "store");
        d_tb->start();
        d_tb->stop();
        d_tb->wait();

        // get buffer pointers
        d_inbuf = d_source->detail()->output(0);
        d_reader = sink->detail()->input(0);

        // vector pointers for input and output
        d_input_items = gr_vector_const_void_star(1);
        d_output_items = gr_vector_void_star(1);
    }

    // public destructor
    ~streaming_interface() {}

    // stop simulator
    bool stop() { return d_sink->stop(); }

    // add tag to input stream
    void add_tag(pmt::pmt_t key, pmt::pmt_t value, uint64_t offset)
    {
        gr::tag_t tag;
        tag.key = key;
        tag.value = value;
        tag.offset = offset;
        d_inbuf->add_item_tag(tag);
    }

    int push(int n, bool do_write = true)
    {
        // update buffer with "data"
        if (do_write)
            d_inbuf->update_write_pointer(n);

        // call work function
        d_input_items[0] = d_reader->read_pointer();
        d_output_items[0] = NULL; // no output for block
        int noutput_items = d_sink->work(n, d_input_items, d_output_items);
        d_reader->update_read_pointer(noutput_items);

        return noutput_items;
    }

    std::vector<pmt::pmt_t> messages()
    {
        // get messages
        std::vector<pmt::pmt_t> messages;
        pmt::pmt_t head = d_debug->delete_head_nowait(pmt::intern("store"));
        while (head != pmt::pmt_t()) {
            messages.push_back(head);
            head = d_debug->delete_head_nowait(pmt::intern("store"));
        }
        return messages;
    }

private:
    // blocks
    gr::sandia_utils::file_source::sptr d_source;
    gr::sandia_utils::file_sink::sptr d_sink;
    gr::blocks::message_debug::sptr d_debug;
    gr::top_block_sptr d_tb;

    // objects
    gr::buffer_sptr d_inbuf;
    gr::buffer_reader_sptr d_reader;
    gr_vector_const_void_star d_input_items;
    gr_vector_void_star d_output_items;
};

// test instantiation of all file data types
BOOST_AUTO_TEST_CASE(t0)
{
    // complex float
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          0,
                                          1000,
                                          "/tmp",
                                          "test"));

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(gr_complex),
                        sink->input_signature()->sizeof_stream_item(0));

    // complex int
    sink = gr::sandia_utils::file_sink::make("complex_int",
                                             2 * sizeof(short),
                                             "raw",
                                             gr::sandia_utils::MANUAL,
                                             0,
                                             1000,
                                             "/tmp",
                                             "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(2 * (int)sizeof(short),
                        sink->input_signature()->sizeof_stream_item(0));

    // float
    sink = gr::sandia_utils::file_sink::make(
        "float", sizeof(float), "raw", gr::sandia_utils::MANUAL, 0, 1000, "/tmp", "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(float),
                        sink->input_signature()->sizeof_stream_item(0));

    // int
    sink = gr::sandia_utils::file_sink::make(
        "int", sizeof(int32_t), "raw", gr::sandia_utils::MANUAL, 0, 1000, "/tmp", "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(int32_t),
                        sink->input_signature()->sizeof_stream_item(0));

    // short
    sink = gr::sandia_utils::file_sink::make(
        "short", sizeof(short), "raw", gr::sandia_utils::MANUAL, 0, 1000, "/tmp", "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(short),
                        sink->input_signature()->sizeof_stream_item(0));

    // byte
    sink = gr::sandia_utils::file_sink::make(
        "byte", sizeof(char), "raw", gr::sandia_utils::MANUAL, 0, 1000, "/tmp", "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(1, sink->input_signature()->max_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->min_streams());
    BOOST_REQUIRE_EQUAL(0, sink->output_signature()->max_streams());
    BOOST_REQUIRE_EQUAL((int)sizeof(char),
                        sink->input_signature()->sizeof_stream_item(0));
}

// test instantiation of all file types
BOOST_AUTO_TEST_CASE(t1)
{
    // raw
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          0,
                                          1000,
                                          "/tmp",
                                          "test"));

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());

    // raw header
    sink = gr::sandia_utils::file_sink::make("complex",
                                             sizeof(gr_complex),
                                             "raw_header",
                                             gr::sandia_utils::MANUAL,
                                             0,
                                             1000,
                                             "/tmp",
                                             "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());

#ifdef HAVE_BLUEFILE_LIB
    // bluefile
    sink = gr::sandia_utils::file_sink::make(
        "complex_int", 4, "bluefile", gr::sandia_utils::MANUAL, 0, 1000, "/tmp", "test");

    BOOST_REQUIRE_EQUAL(std::string("file_sink"), sink->name());
#endif
}

// test streaming to a single file
BOOST_AUTO_TEST_CASE(t2)
{
    // setup sink to generate a single file
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          0,
                                          1000,
                                          "/tmp",
                                          "test.fc32"));
    // set sink to start recording
    sink->set_second_align(false);
    sink->set_gen_new_folder(false);
    sink->set_recording(true);

    // streaming interface simulator
    streaming_interface sim(sink);

    // "push" data to sink
    int noutput_items = sim.push(4000);
    BOOST_REQUIRE_EQUAL(4000, noutput_items);

    // stop recording
    sim.stop();

    // collect messages
    std::vector<pmt::pmt_t> messages = sim.messages();
    BOOST_REQUIRE_EQUAL(1, messages.size());

    // expected pmt
    BOOST_REQUIRE(pmt::is_pdu(messages[0]));
    pmt::pmt_t meta = pmt::car(messages[0]);
    BOOST_REQUIRE(pmt::equal(pmt::from_double(1000),
                             pmt::dict_ref(meta, PMTCONSTSTR__rx_rate(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::from_double(0),
                             pmt::dict_ref(meta, PMTCONSTSTR__rx_freq(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::intern("/tmp/test.fc32"),
                             pmt::dict_ref(meta, PMTCONSTSTR__fname(), pmt::PMT_NIL)));
    BOOST_REQUIRE(
        pmt::equal(pmt::from_uint64(8),
                   pmt::dict_ref(meta, PMTCONSTSTR__item_size(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::from_uint64(4000),
                             pmt::dict_ref(meta, PMTCONSTSTR__samples(), pmt::PMT_NIL)));


    // clean up
    remove_file("/tmp/test.fc32");
}

// test tag name generation
BOOST_AUTO_TEST_CASE(t3)
{
    // generate sink block to generate filename based on collection parameters
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          0,
                                          1000,
                                          "/tmp",
                                          "%Y%m%d_%H_%M_%S_fc=%fcMMHz_fs=%fskkHz.dat"));
    // set sink to start recording
    sink->set_second_align(false);
    sink->set_gen_new_folder(false);
    sink->set_recording(true);

    // streaming interface simulator
    streaming_interface sim(sink);

    // add stream tags
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_rate(), pmt::from_double(30.72e6), 0);
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_freq(), pmt::from_double(915e6), 0);
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_time(),
                pmt::make_tuple(pmt::from_uint64(0), pmt::from_double(0)),
                0);

    // "push" some data through sink
    sim.push(1000);

    // stop simulator
    sim.stop();

    // collect results
    std::vector<pmt::pmt_t> messages = sim.messages();
    BOOST_REQUIRE_EQUAL(1, messages.size());
    BOOST_REQUIRE(pmt::is_pdu(messages[0]));
    pmt::pmt_t dict = pmt::car(messages[0]);
    BOOST_REQUIRE_EQUAL(
        pmt::equal(
            pmt::dict_ref(dict, gr::sandia_utils::PMTCONSTSTR__rx_rate(), pmt::PMT_NIL),
            pmt::from_double(30.72e6)),
        true);
    BOOST_REQUIRE_EQUAL(
        pmt::equal(
            pmt::dict_ref(dict, gr::sandia_utils::PMTCONSTSTR__rx_freq(), pmt::PMT_NIL),
            pmt::from_double(915e6)),
        true);
    BOOST_REQUIRE_EQUAL(
        pmt::equal(
            pmt::dict_ref(dict, gr::sandia_utils::PMTCONSTSTR__rx_time(), pmt::PMT_NIL),
            pmt::make_tuple(pmt::from_uint64(0), pmt::from_double(0))),
        true);

    std::string expected_fname = "/tmp/19700101_00_00_00_fc=915MHz_fs=30720kHz.dat";
    BOOST_REQUIRE_EQUAL(
        pmt::equal(
            pmt::dict_ref(dict, gr::sandia_utils::PMTCONSTSTR__fname(), pmt::PMT_NIL),
            pmt::intern(expected_fname)),
        true);

    // clean up
    remove_file(expected_fname);
}

// validate that a tag on the first sample does not result
// in a second file notice being published
BOOST_AUTO_TEST_CASE(t4)
{

    // generate sink
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          0,
                                          1000,
                                          "/tmp",
                                          "test.fc32"));
    // set sink to start recording
    sink->set_second_align(false);
    sink->set_gen_new_folder(false);
    sink->set_recording(true);

    // streaming interface simulator
    streaming_interface sim(sink);

    // use a vector source block to place two tags in the stream
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_rate(), pmt::from_double(30.72e6), 0);
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_freq(), pmt::from_double(915e6), 0);
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_time(),
                pmt::make_tuple(pmt::from_uint64(0), pmt::from_double(0)),
                0);

    // "push" some data
    //int noutput_items = sim.push(4000);
    sim.push(4000);

    // stop recording so message will be published
    sim.stop();

    // collect messages
    std::vector<pmt::pmt_t> messages = sim.messages();
    BOOST_REQUIRE_EQUAL(1, messages.size());

    // expected pmt
    BOOST_REQUIRE(pmt::is_pdu(messages[0]));
    pmt::pmt_t meta = pmt::car(messages[0]);
    BOOST_REQUIRE(pmt::equal(pmt::from_double(30.72e6),
                             pmt::dict_ref(meta, PMTCONSTSTR__rx_rate(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::from_double(915e6),
                             pmt::dict_ref(meta, PMTCONSTSTR__rx_freq(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::intern("/tmp/test.fc32"),
                             pmt::dict_ref(meta, PMTCONSTSTR__fname(), pmt::PMT_NIL)));
    BOOST_REQUIRE(
        pmt::equal(pmt::from_uint64(8),
                   pmt::dict_ref(meta, PMTCONSTSTR__item_size(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::from_uint64(4000),
                             pmt::dict_ref(meta, PMTCONSTSTR__samples(), pmt::PMT_NIL)));

    // clean up
    remove_file("/tmp/test.fc32");
}


// test generation of multiple files based on number of samples in file
// note: because the simulator "stops" the sink before we simulating, the
// current file names are off by one due to the automatic file number
// increment
BOOST_AUTO_TEST_CASE(t5)
{
    // generate blocks
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          2000,
                                          2000,
                                          "/tmp",
                                          "t_%02fd.fc32"));

    // set sink to start recording
    sink->set_second_align(false);
    sink->set_gen_new_folder(false);
    sink->set_recording(true);

    // streaming interface simulator
    streaming_interface sim(sink);

    // "process" enough data for two files
    int noutput_items = sim.push(3000);
    BOOST_REQUIRE_EQUAL(noutput_items, 3000);

    // stop simulation
    sim.stop();

    // collect messages
    std::vector<pmt::pmt_t> messages = sim.messages();
    BOOST_REQUIRE_EQUAL(2, messages.size());

    // expected pmt
    BOOST_REQUIRE(pmt::is_pdu(messages[0]));
    BOOST_REQUIRE(pmt::is_pdu(messages[1]));

    // verify rx timestamps relative to start and name of each file
    pmt::pmt_t dict1 = pmt::car(messages[0]);
    pmt::pmt_t dict2 = pmt::car(messages[1]);
    pmt::pmt_t time_tuple1 =
        pmt::dict_ref(dict1, gr::sandia_utils::PMTCONSTSTR__rx_time(), pmt::PMT_NIL);
    uint64_t tstart1 = pmt::to_uint64(pmt::tuple_ref(time_tuple1, 0));

    pmt::pmt_t time_tuple2 =
        pmt::dict_ref(dict2, gr::sandia_utils::PMTCONSTSTR__rx_time(), pmt::PMT_NIL);
    uint64_t tstart2 = pmt::to_uint64(pmt::tuple_ref(time_tuple2, 0));
    BOOST_REQUIRE_EQUAL(tstart2, tstart1 + 1);
    BOOST_REQUIRE(pmt::equal(pmt::intern("/tmp/t_01.fc32"),
                             pmt::dict_ref(dict1, PMTCONSTSTR__fname(), pmt::PMT_NIL)));
    BOOST_REQUIRE(pmt::equal(pmt::intern("/tmp/t_02.fc32"),
                             pmt::dict_ref(dict2, PMTCONSTSTR__fname(), pmt::PMT_NIL)));

    // clean up
    BOOST_REQUIRE(remove_file("/tmp/t_01.fc32"));
    BOOST_REQUIRE(remove_file("/tmp/t_02.fc32"));
}

// test a tag in the middle of a work function call to ensure that the number
// of samples in the cut file is correct
BOOST_AUTO_TEST_CASE(t6)
{
    // generate sink with a set number of samples per file
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          4000,
                                          1000,
                                          "/tmp",
                                          "t_%02fd.fc32"));
    // set sink to start recording
    sink->set_second_align(false);
    sink->set_gen_new_folder(false);
    sink->set_recording(true);

    // streaming interface simulator
    streaming_interface sim(sink);

    // add tags into the stream on the first sample
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_rate(), pmt::from_double(30.72e6), 0);
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_freq(), pmt::from_double(915e6), 0);
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_time(),
                pmt::make_tuple(pmt::from_uint64(0), pmt::from_double(0)),
                0);

    // add a second tag with an updated time to simulate an overflow
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_time(),
                pmt::make_tuple(pmt::from_uint64(1), pmt::from_double(0)),
                2000);

    // push a small chunk of data without a tag through
    int noutput_items = sim.push(1500);
    BOOST_REQUIRE_EQUAL(1500, noutput_items);

    // try to push the next block through but should only process up
    // to the tag
    noutput_items = sim.push(1000, false);
    BOOST_REQUIRE_EQUAL(500, noutput_items);

    // push the next block of data through
    noutput_items = sim.push(1000, false);
    BOOST_REQUIRE_EQUAL(1000, noutput_items);

    // stop the simulator
    sim.stop();

    // collect messages
    std::vector<pmt::pmt_t> messages = sim.messages();
    BOOST_REQUIRE_EQUAL(2, messages.size());

    // expected pmt
    BOOST_REQUIRE(pmt::is_pdu(messages[0]));
    BOOST_REQUIRE(pmt::is_pdu(messages[1]));

    // verify rx timestamps relative to start and name of each file
    pmt::pmt_t dict1 = pmt::car(messages[0]);
    pmt::pmt_t dict2 = pmt::car(messages[1]);

    // validate time of messages and the number of samples in each file
    pmt::pmt_t time_tuple1 =
        pmt::dict_ref(dict1, gr::sandia_utils::PMTCONSTSTR__rx_time(), pmt::PMT_NIL);
    pmt::pmt_t time_tuple2 =
        pmt::dict_ref(dict2, gr::sandia_utils::PMTCONSTSTR__rx_time(), pmt::PMT_NIL);
    uint64_t tstart1 = pmt::to_uint64(pmt::tuple_ref(time_tuple1, 0));
    uint64_t tstart2 = pmt::to_uint64(pmt::tuple_ref(time_tuple2, 0));
    uint64_t nsamples1 = pmt::to_uint64(pmt::dict_ref(
        dict1, gr::sandia_utils::PMTCONSTSTR__samples(), pmt::from_uint64(0)));
    uint64_t nsamples2 = pmt::to_uint64(pmt::dict_ref(
        dict2, gr::sandia_utils::PMTCONSTSTR__samples(), pmt::from_uint64(0)));
    BOOST_REQUIRE_EQUAL(tstart1, uint64_t(0));
    BOOST_REQUIRE_EQUAL(tstart2, uint64_t(1));
    BOOST_REQUIRE_EQUAL(nsamples1, uint64_t(2000));
    BOOST_REQUIRE_EQUAL(nsamples2, uint64_t(1000));

    // clean up
    BOOST_REQUIRE(remove_file("/tmp/t_01.fc32"));
    BOOST_REQUIRE(remove_file("/tmp/t_02.fc32"));
}

// test streaming with tags on first sample of work function call
// https://gitlab.sandia.gov/sandia-sdr/gnuradio_common/gr-sandia_utils/issues/16
BOOST_AUTO_TEST_CASE(t7)
{
    // setup sink to generate multiple files of 4000 samples each
    gr::sandia_utils::file_sink::sptr sink(
        gr::sandia_utils::file_sink::make("complex",
                                          sizeof(gr_complex),
                                          "raw",
                                          gr::sandia_utils::MANUAL,
                                          4000,
                                          1000,
                                          "/tmp",
                                          "t_%02fd.fc32"));
    sink->set_recording(true);
    sink->set_second_align(false);
    sink->set_gen_new_folder(false);

    // streaming interface simulator
    streaming_interface sim(sink);

    // add initial tag
    sim.add_tag(gr::sandia_utils::PMTCONSTSTR__rx_rate(), pmt::from_double(30.72e6), 0);

    // add a second tag that will be on the first sample of the second
    // work function call.
    sim.add_tag(
        gr::sandia_utils::PMTCONSTSTR__rx_rate(), pmt::from_double(30.72e6), 1000);


    // "process" data
    int noutput_items = sim.push(1000);
    BOOST_REQUIRE_EQUAL(noutput_items, 1000);

    // call work function again so second tag is consumed
    // but don't push anymore data into the input fifo
    noutput_items = sim.push(1000, false);
    BOOST_REQUIRE_EQUAL(noutput_items, 0);

    // last work function call should have returned zero
    noutput_items = sim.push(1000, false);
    BOOST_REQUIRE_EQUAL(noutput_items, 1000);

    // stop simulator
    BOOST_REQUIRE_EQUAL(sim.stop(), true);

    // get messages
    std::vector<pmt::pmt_t> messages = sim.messages();
    BOOST_REQUIRE_EQUAL(2, messages.size());

    // remove files
    remove_file("/tmp/t_00.fc32"); // never actually written due to first stop call
    BOOST_REQUIRE_EQUAL(remove_file("/tmp/t_01.fc32"), true);
    BOOST_REQUIRE_EQUAL(remove_file("/tmp/t_02.fc32"), true);
}
} // namespace sandia_utils
} // namespace gr
