#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2019 gr-sandia_utils author.
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
