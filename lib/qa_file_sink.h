#ifndef INCLUDED_QA_FILE_SINK_H
#define INCLUDED_QA_FILE_SINK_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>
#include <string>
#include <gnuradio/tags.h>

class qa_file_sink : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(qa_file_sink);

    CPPUNIT_TEST(t0);
    CPPUNIT_TEST(t1);
    CPPUNIT_TEST(t2);
    CPPUNIT_TEST(t3);
    CPPUNIT_TEST(t4);
    CPPUNIT_TEST(t5);
    CPPUNIT_TEST(t6);
    CPPUNIT_TEST(t7);

    CPPUNIT_TEST_SUITE_END();

private:
    void t0();
    void t1();
    void t2();
    void t3();
    void t4();
    void t5();
    void t6();
    void t7();

    // tag generator
    gr::tag_t gen_tag(pmt::pmt_t key, pmt::pmt_t value, uint64_t offset);

    // cleanup
    bool remove_file(std::string);
};

#endif /* INCLUDED_QA_FILE_SINK_H */
