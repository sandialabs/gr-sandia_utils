#!/usr/bin/env python
#
# Copyright 2012-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from gnuradio import gr, gr_unittest, blocks
try:
    from gnuradio import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import sandia_utils

import pmt
import time


class qa_sandia_tag_debug(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_notags(self):
        '''
        Tests a stream with no tags
        '''
        # Just run some data through and make sure it doesn't puke.
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data)
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src, dut)

        self.tb.run()

        self.assertEqual(0, dut.num_tags())

    def test_002_1tag(self):
        '''
        Tests a stream with a single tag
        '''

        src_tag = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_002_1tag")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data, False, 1, [src_tag])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src, dut)

        self.tb.run()

        self.assertEqual(1, dut.num_tags())

        tag0 = dut.get_tag(0)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("sam")))
        self.assertAlmostEqual(10000, pmt.to_double(tag0.value))

    def test_003_tags(self):
        '''
        Tests a stream that has multiple tags inside it
        '''
        src_tag1 = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_003_tags")])
        src_tag2 = gr.tag_utils.python_to_tag([1, pmt.intern("peter"), pmt.from_double(1000), pmt.intern("test_003_tags")])
        src_tag3 = gr.tag_utils.python_to_tag([2, pmt.intern("jacob"), pmt.from_double(100), pmt.intern("test_003_tags")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data, False, 1, [src_tag1, src_tag2, src_tag3])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src, dut)

        self.tb.run()

        self.assertEqual(3, dut.num_tags())

        tag0 = dut.get_tag(0)
        tag1 = dut.get_tag(1)
        tag2 = dut.get_tag(2)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("sam")))
        self.assertAlmostEqual(10000, pmt.to_double(tag0.value))

        self.assertTrue(pmt.eq(tag1.key, pmt.intern("peter")))
        self.assertAlmostEqual(1000, pmt.to_double(tag1.value))

        self.assertTrue(pmt.eq(tag2.key, pmt.intern("jacob")))
        self.assertAlmostEqual(100, pmt.to_double(tag2.value))

    def test_004_cleartags(self):
        '''
        testing sandia_tag_debug::clear_tags() function
        '''
        src_tag1 = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_003_tags")])
        src_tag2 = gr.tag_utils.python_to_tag([1, pmt.intern("peter"), pmt.from_double(1000), pmt.intern("test_003_tags")])
        src_tag3 = gr.tag_utils.python_to_tag([2, pmt.intern("jacob"), pmt.from_double(100), pmt.intern("test_003_tags")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data, False, 1, [src_tag1, src_tag2, src_tag3])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src, dut)

        self.tb.run()

        self.assertEqual(3, dut.num_tags())
        dut.clear_tags()
        self.assertEqual(0, dut.num_tags())

    def test_005_multiWork(self):
        '''
        This test is testing multiple calls to the sandia_tag_debug::work function 
        to ensure tags are all being saved. 
        '''
        src_tag1 = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_003_tags")])
        src_tag2 = gr.tag_utils.python_to_tag([1, pmt.intern("peter"), pmt.from_double(1000), pmt.intern("test_003_tags")])
        src_tag3 = gr.tag_utils.python_to_tag([2, pmt.intern("jacob"), pmt.from_double(100), pmt.intern("test_003_tags")])
        src_tag4 = gr.tag_utils.python_to_tag([2, pmt.intern("chip"), pmt.from_double(10), pmt.intern("test_003_tags")])
        src_tag5 = gr.tag_utils.python_to_tag([2, pmt.intern("josh"), pmt.from_double(1), pmt.intern("test_003_tags")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data, False, 1, [src_tag1, src_tag2, src_tag3])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src, dut)

        #Run one of the TB
        self.tb.run()

        self.assertEqual(3, dut.num_tags())

        tag0 = dut.get_tag(0)
        tag1 = dut.get_tag(1)
        tag2 = dut.get_tag(2)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("sam")))
        self.assertAlmostEqual(10000, pmt.to_double(tag0.value))

        self.assertTrue(pmt.eq(tag1.key, pmt.intern("peter")))
        self.assertAlmostEqual(1000, pmt.to_double(tag1.value))

        self.assertTrue(pmt.eq(tag2.key, pmt.intern("jacob")))
        self.assertAlmostEqual(100, pmt.to_double(tag2.value))

        self.tb.stop()
        self.tb.wait()

        #Run two of the TB
        src.set_data(src_data, [src_tag4, src_tag5])
        self.tb.run()

        self.assertEqual(5, dut.num_tags())

        tag3 = dut.get_tag(3)
        tag4 = dut.get_tag(4)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("sam")))
        self.assertAlmostEqual(10000, pmt.to_double(tag0.value))

        self.assertTrue(pmt.eq(tag1.key, pmt.intern("peter")))
        self.assertAlmostEqual(1000, pmt.to_double(tag1.value))

        self.assertTrue(pmt.eq(tag2.key, pmt.intern("jacob")))
        self.assertAlmostEqual(100, pmt.to_double(tag2.value))

        self.assertTrue(pmt.eq(tag3.key, pmt.intern("chip")))
        self.assertAlmostEqual(10, pmt.to_double(tag3.value))

        self.assertTrue(pmt.eq(tag4.key, pmt.intern("josh")))
        self.assertAlmostEqual(1, pmt.to_double(tag4.value))

        self.tb.stop()
        self.tb.wait()

    def test_006_filter(self):
        '''
        Test a sandia_tag_debug with a filter string
        '''
        src_tag1 = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_003_tags")])
        src_tag2 = gr.tag_utils.python_to_tag([1, pmt.intern("peter"), pmt.from_double(1000), pmt.intern("test_003_tags")])
        src_tag3 = gr.tag_utils.python_to_tag([2, pmt.intern("jacob"), pmt.from_double(100), pmt.intern("test_003_tags")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data, False, 1, [src_tag1, src_tag2, src_tag3])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA", "peter")
        self.tb.connect(src, dut)

        self.tb.run()

        self.assertEqual(1, dut.num_tags())

        tag0 = dut.get_tag(0)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("peter")))
        self.assertAlmostEqual(1000, pmt.to_double(tag0.value))

    def test_007_dual_notags(self):
        '''
        Tests 2 inbound streams with no tags
        '''
        # Just run some data through and make sure it doesn't puke.
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src1 = blocks.vector_source_i(src_data)
        src2 = blocks.vector_source_i(src_data)
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src1, (dut, 0))
        self.tb.connect(src2, (dut, 1))

        self.tb.run()

        self.assertEqual(0, dut.num_tags())

    def test_008_dual_tags(self):
        '''
        This test has 2 sources each with tags 
        '''
        src_tag1 = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_003_tags")])
        src_tag2 = gr.tag_utils.python_to_tag([1, pmt.intern("peter"), pmt.from_double(1000), pmt.intern("test_003_tags")])
        src_tag3 = gr.tag_utils.python_to_tag([2, pmt.intern("jacob"), pmt.from_double(100), pmt.intern("test_003_tags")])
        src_tag4 = gr.tag_utils.python_to_tag([2, pmt.intern("chip"), pmt.from_double(10), pmt.intern("test_003_tags")])
        src_tag5 = gr.tag_utils.python_to_tag([2, pmt.intern("josh"), pmt.from_double(1), pmt.intern("test_003_tags")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src1 = blocks.vector_source_i(src_data, False, 1, [src_tag1, src_tag2, src_tag3])
        src2 = blocks.vector_source_i(src_data, False, 1, [src_tag4, src_tag5])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src1, (dut, 0))
        self.tb.connect(src2, (dut, 1))

        self.tb.run()

        self.assertEqual(5, dut.num_tags())

        tag0 = dut.get_tag(0)
        tag1 = dut.get_tag(1)
        tag2 = dut.get_tag(2)
        tag3 = dut.get_tag(3)
        tag4 = dut.get_tag(4)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("sam")))
        self.assertAlmostEqual(10000, pmt.to_double(tag0.value))

        self.assertTrue(pmt.eq(tag1.key, pmt.intern("peter")))
        self.assertAlmostEqual(1000, pmt.to_double(tag1.value))

        self.assertTrue(pmt.eq(tag2.key, pmt.intern("jacob")))
        self.assertAlmostEqual(100, pmt.to_double(tag2.value))

        self.assertTrue(pmt.eq(tag3.key, pmt.intern("chip")))
        self.assertAlmostEqual(10, pmt.to_double(tag3.value))

        self.assertTrue(pmt.eq(tag4.key, pmt.intern("josh")))
        self.assertAlmostEqual(1, pmt.to_double(tag4.value))

        self.tb.stop()
        self.tb.wait()

    def test_009_dual_tags_nostore(self):
        '''
        This test has 2 sources each with tags 
        '''
        src_tag1 = gr.tag_utils.python_to_tag([0, pmt.intern("sam"), pmt.from_double(10000), pmt.intern("test_003_tags")])
        src_tag2 = gr.tag_utils.python_to_tag([1, pmt.intern("peter"), pmt.from_double(1000), pmt.intern("test_003_tags")])
        src_tag3 = gr.tag_utils.python_to_tag([2, pmt.intern("jacob"), pmt.from_double(100), pmt.intern("test_003_tags")])
        src_tag4 = gr.tag_utils.python_to_tag([2, pmt.intern("chip"), pmt.from_double(10), pmt.intern("test_003_tags")])
        src_tag5 = gr.tag_utils.python_to_tag([2, pmt.intern("josh"), pmt.from_double(1), pmt.intern("test_003_tags")])

        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src1 = blocks.vector_source_i(src_data, False, 1, [src_tag1, src_tag2, src_tag3])
        src2 = blocks.vector_source_i(src_data, False, 1, [src_tag4, src_tag5])
        dut = sandia_utils.sandia_tag_debug(gr.sizeof_int, "tag QA", "", False)
        self.tb.connect(src1, (dut, 0))
        self.tb.connect(src2, (dut, 1))

        self.tb.run()

        self.assertEqual(5, dut.num_tags())

        tag0 = dut.get_tag(0)
        tag1 = dut.get_tag(1)
        tag2 = dut.get_tag(2)
        tag3 = dut.get_tag(3)
        tag4 = dut.get_tag(4)

        self.assertTrue(pmt.eq(tag0.key, pmt.intern("sam")))
        self.assertAlmostEqual(10000, pmt.to_double(tag0.value))

        self.assertTrue(pmt.eq(tag1.key, pmt.intern("peter")))
        self.assertAlmostEqual(1000, pmt.to_double(tag1.value))

        self.assertTrue(pmt.eq(tag2.key, pmt.intern("jacob")))
        self.assertAlmostEqual(100, pmt.to_double(tag2.value))

        self.assertTrue(pmt.eq(tag3.key, pmt.intern("chip")))
        self.assertAlmostEqual(10, pmt.to_double(tag3.value))

        self.assertTrue(pmt.eq(tag4.key, pmt.intern("josh")))
        self.assertAlmostEqual(1, pmt.to_double(tag4.value))

        self.tb.stop()
        self.tb.wait()


if __name__ == '__main__':
    gr_unittest.run(qa_sandia_tag_debug, "qa_sandia_tag_debug.xml")
