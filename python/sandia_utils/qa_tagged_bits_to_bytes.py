#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
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


class qa_tagged_bits_to_bytes(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_one_tag_aligned(self):
        # data
        src_data = (1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 0
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test one tag aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_one_tag_not_aligned(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 2
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test one tag not aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_two_tags_first_aligned(self):
        # data
        src_data = (1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 0
        src_tag1 = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        offset = 8
        src_tag2 = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag1, src_tag2])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test two tags first aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_two_tags_not_aligned(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 2
        src_tag1 = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        offset = 10
        src_tag2 = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag1, src_tag2])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test two tags not aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_big_endian_two_tags_not_aligned(self):
        # data
        src_data = (0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1)
        offset = 2
        src_tag1 = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        offset = 10
        src_tag2 = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag1, src_tag2])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", True, 0, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test big endian two tags not aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_vector_one_tag_not_aligned(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 2
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd, 0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        v_len = 2
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, v_len)
        dst = blocks.vector_sink_b(v_len)
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test vector output two tags not aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_big_vector_one_tag_not_aligned(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 2
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = () # vector is too big, should never get an output

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        v_len = 8
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, v_len)
        dst = blocks.vector_sink_b(v_len)
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.start()
        time.sleep(.005)
        self.tb.stop()
        result_data = dst.data()

        # assert
        #print("test big vector output two tags not aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_one_tag_not_bye_aligned(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1)
        offset = 2
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        # This test will run forever, so we need to stop it manually
        self.tb.start()
        time.sleep(.005)
        self.tb.stop()
        result_data = dst.data()

        # assert
        #print("test one tag not byte aligned got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_pad_left(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 2
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0x01, 0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, sandia_utils.PAD_LEFT, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test pad left got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_pad_right(self):
        # data
        src_data = (0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1)
        offset = 2
        src_tag = gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")])
        expected_result = (0x40, 0xab, 0xcd)

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, [src_tag])
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, sandia_utils.PAD_RIGHT, 1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, tbb)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        #print("test pad right got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_rx_time_drop(self):
        # data
        src_data = (0,) * 8 * 100
        offsets = [16]
        for step in range(17, 24):
            offsets.append(offsets[-1] + step)
        print("offsets = {}".format(offsets))

        # generate tag list
        rx_time = pmt.make_tuple(pmt.from_uint64(1), pmt.from_double(.234))
        time_tag = gr.tag_utils.python_to_tag([1, pmt.intern("rx_time"), rx_time, pmt.intern("time_stamper")])
        tags = [time_tag]
        for offset in offsets:
            tags.append(gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")]))

        expected_result = (0x0,) * 5

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, tags)
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 0, 1)
        dst = blocks.vector_sink_b()
        tag_dbg = blocks.tag_debug(gr.sizeof_char * 1, '', "")
        tag_dbg.set_display(True)

        self.tb.connect(src, tbb)
        self.tb.connect(tbb, tag_dbg)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert - should get both a BURST and burst_time tag at offsets = [2,4,8,10,12,14,16]
        print("test rx_time got {}, expected {}".format(result_data, expected_result))
        # self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_rx_time_pad_right(self):
        # data
        src_data = (0,) * 8 * 100
        offsets = [16]
        for step in range(17, 24):
            offsets.append(offsets[-1] + step)
        print("offsets = {}".format(offsets))

        # generate tag list
        rx_time = pmt.make_tuple(pmt.from_uint64(1), pmt.from_double(.234))
        time_tag = gr.tag_utils.python_to_tag([1, pmt.intern("rx_time"), rx_time, pmt.intern("time_stamper")])
        tags = [time_tag]
        for offset in offsets:
            tags.append(gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")]))

        expected_result = (0x0,) * 5

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, tags)
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 1, 1)
        dst = blocks.vector_sink_b()
        tag_dbg = blocks.tag_debug(gr.sizeof_char * 1, '', "")
        tag_dbg.set_display(True)

        self.tb.connect(src, tbb)
        self.tb.connect(tbb, tag_dbg)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert - should get both a BURST and burst_time tag at offsets = [2,5,8,11,14,17,20,23]
        print("test rx_time got {}, expected {}".format(result_data, expected_result))
        # self.assertEqual(tuple(expected_result), tuple(result_data))

    def test_rx_time_pad_left(self):
        # data
        src_data = (0,) * 8 * 100
        offsets = [16]
        for step in range(17, 24):
            offsets.append(offsets[-1] + step)
        print("offsets = {}".format(offsets))

        # generate tag list
        rx_time = pmt.make_tuple(pmt.from_uint64(1), pmt.from_double(.234))
        time_tag = gr.tag_utils.python_to_tag([1, pmt.intern("rx_time"), rx_time, pmt.intern("time_stamper")])
        tags = [time_tag]
        for offset in offsets:
            tags.append(gr.tag_utils.python_to_tag([offset, pmt.intern("BURST"), pmt.from_uint64(0), pmt.intern("test_simple_source")]))

        expected_result = (0x0,) * 5

        # blocks
        src = blocks.vector_source_b(src_data, False, 1, tags)
        tbb = sandia_utils.tagged_bits_to_bytes("BURST", False, 2, 1)
        dst = blocks.vector_sink_b()
        tag_dbg = blocks.tag_debug(gr.sizeof_char * 1, '', "")
        tag_dbg.set_display(True)

        self.tb.connect(src, tbb)
        self.tb.connect(tbb, tag_dbg)
        self.tb.connect(tbb, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert - should get both a BURST and burst_time tag at offsets = [2,5,8,11,14,17,20,23]
        print("test rx_time got {}, expected {}".format(result_data, expected_result))
        # self.assertEqual(tuple(expected_result), tuple(result_data))


if __name__ == '__main__':
    gr_unittest.run(qa_tagged_bits_to_bytes)
