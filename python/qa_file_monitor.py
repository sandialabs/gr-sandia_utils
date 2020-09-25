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
import pdu_utils
import pmt
import time
from file_monitor import file_monitor
import os
from pathlib import Path

class qa_file_archiver(gr_unittest.TestCase):

    def setUp(self):
      self.tb = gr.top_block()

    def tearDown(self):
      self.tb = None

    def test1_cleanup(self):
      '''
      All files should be deleted by the monitor when complete
      '''
      # open a dummy file
      fname = '/tmp/foo.txt'
      if os.path.exists(fname):
        os.remove(fname)
      Path(fname).touch()

      # PMT
      p = pmt.dict_add(pmt.make_dict(),pmt.intern('rx_freq'), pmt.from_double(915e6))
      p = pmt.dict_add(p,pmt.intern('rx_rate'),pmt.from_double(30.72e6))
      p = pmt.dict_add(p,pmt.intern('rx_time'),pmt.make_tuple(
        pmt.from_uint64(0),pmt.from_double(0)))
      p = pmt.dict_add(p,pmt.intern('fname'),pmt.intern(fname))

      # blocks
      emitter = pdu_utils.message_emitter(pmt.PMT_NIL)
      debug = blocks.message_debug()
      monitor = file_monitor(10,'/tmp')

      # connect
      self.tb.msg_connect((emitter,'msg'),(monitor,'pdu'))

      # set up fg - terrible hacky way of doing this until we get
      # pdu utility message emitter working
      self.tb.start()
      emitter.emit(p)
      time.sleep(.05)
      self.tb.stop()
      self.tb.wait()

      # check data
      self.assertTrue(not os.path.exists(fname))

if __name__ == '__main__':
    gr_unittest.run(qa_file_archiver)
