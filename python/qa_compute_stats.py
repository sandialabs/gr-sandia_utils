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
import sandia_utils_swig as sandia_utils
import pdu_utils
import pmt
import time
import numpy as np

class qa_compute_stats(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.emitter = pdu_utils.message_emitter()
        self.debug = blocks.message_debug()
        self.compute = sandia_utils.compute_stats()
        self.tb.msg_connect((self.emitter, 'msg'), (self.compute, 'pdu_in'))
        self.tb.msg_connect((self.compute, 'pdu_out'), (self.debug, 'store'))

    def tearDown(self):
        self.tb = None

    def test_simple(self):
      # make the data and expected results
      data = [x+x*1j for x in range(10)]
      pdu_in = pmt.cons(pmt.make_dict(), pmt.init_c32vector(len(data),data))
      expected_energy = sum([abs(x)**2 for x in data])
      expected_power = 10 * np.log10(expected_energy / len(data))

      # run flowgraph
      self.tb.start()
      time.sleep(.001)
      self.emitter.emit(pmt.intern("BAD PDU"))
      time.sleep(.001)
      self.emitter.emit(pdu_in)
      time.sleep(.01)
      self.tb.stop()
      self.tb.wait() # don't wait...may not return in time

      # extract results
      rcv_pdu = self.debug.get_message(0)
      rcv_meta = pmt.car(rcv_pdu)
      rcv_data = pmt.c32vector_elements(pmt.cdr(rcv_pdu))
      rcv_energy = pmt.to_double(pmt.dict_ref(rcv_meta, pmt.intern("energy"), pmt.PMT_NIL))
      rcv_power  = pmt.to_double(pmt.dict_ref(rcv_meta, pmt.intern("power"), pmt.PMT_NIL))

      # assert expectations
      precision = 1e-3
      self.assertTrue( abs(rcv_energy - expected_energy) < precision)
      self.assertTrue( abs(rcv_power - expected_power) < precision)


if __name__ == '__main__':
    gr_unittest.run(qa_compute_stats)
