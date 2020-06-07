#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 gr-sandia_utils author.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import sandia_utils_swig as sandia_utils
import pmt
import time

class qa_tagged_bits_to_bytes(gr_unittest.TestCase):

  def setUp (self):
      self.tb = gr.top_block ()

  def tearDown (self):
      self.tb = None

  def test_one_tag_aligned(self):
      # data
      src_data = (1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_one_tag_not_aligned(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_two_tags_first_aligned(self):
      # data
      src_data = (1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_two_tags_not_aligned(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_big_endian_two_tags_not_aligned(self):
      # data
      src_data = (0,1, 1,1,0,1, 0,1,0,1, 1,0,1,1, 0,0,1,1)
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
      self.assertEqual(expected_result, result_data)

  def test_vector_one_tag_not_aligned(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_big_vector_one_tag_not_aligned(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_one_tag_not_bye_aligned(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1, 1)
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
      self.assertEqual(expected_result, result_data)

  def test_pad_left(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

  def test_pad_right(self):
      # data
      src_data = (0,1, 1,0,1,0, 1,0,1,1, 1,1,0,0, 1,1,0,1)
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
      self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(qa_tagged_bits_to_bytes)
