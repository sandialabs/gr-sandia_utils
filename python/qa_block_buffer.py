#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import time
from gnuradio import gr, gr_unittest
from gnuradio import blocks, analog
import sandia_utils_swig as sandia_utils

class qa_block_buffer(gr_unittest.TestCase):

    def setUp(self):
      self.tb = gr.top_block()

    def tearDown(self):
      self.tb = None

    def test_1(self):
      '''
      Very rough test to ensure that the block can be instantiated and
      data can be run
      '''
      samp_rate = samp_rate = 32000
      freq = freq = samp_rate / 10

      ##################################################
      # Blocks
      ##################################################
      block_buffer = sandia_utils.block_buffer(gr.sizeof_gr_complex, 10000, samp_rate, True)
      throttle = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate / 20,True)
      tag_debug = blocks.tag_debug(gr.sizeof_gr_complex*1, '', "")
      tag_debug.set_display(True)
      probe_rate = blocks.probe_rate(gr.sizeof_gr_complex*1, 500.0, 0.15)
      message_debug = blocks.message_debug()
      source = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, freq, .7, 0, 0)

      ##################################################
      # Connections
      ##################################################
      self.tb.msg_connect((probe_rate, 'rate'), (message_debug, 'store'))
      self.tb.connect((source, 0), (block_buffer, 0))
      self.tb.connect((throttle, 0), (probe_rate, 0))
      self.tb.connect((throttle, 0), (tag_debug, 0))
      self.tb.connect((block_buffer, 0), (throttle, 0))

      # data
      src_data = (1,1,2,2,3,3)
      expected_result = (1+1j,2+2j,3+3j)

      # execute
      self.tb.start()

      # allow it to run 2 seconds
      time.sleep(3)

      # get number of messages
      nmessages = message_debug.num_messages()

      # should have gotten at least one message
      self.assertTrue(nmessages > 0)

      # clean up
      self.tb.stop()
      self.tb.wait()



if __name__ == '__main__':
    gr_unittest.run(qa_block_buffer, "qa_block_buffer.xml")
