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
import pmt
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


class qa_rftap_encap(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # set up data
        srcdata = (1, 2, 3)
        meta = pmt.to_pmt({'dissector': 'rds'})
        srcpdu = pmt.cons(meta, pmt.init_u8vector(len(srcdata), srcdata))
        outdata = (82, 70, 116, 97, 4, 0, 0, 0, 16, 0, 3, 255, 114, 100, 115, 0) + srcdata
        outpdu = pmt.cons(meta, pmt.init_u8vector(len(outdata), outdata))

        # set up flowgraph
        encap = sandia_utils.rftap_encap(1, -1, "")
        sink = blocks.message_debug()
        tb = gr.top_block()
        tb.msg_connect(encap, "out", sink, "store")

        # run flowgraph
        tb.start()
        t = time.time()
        encap.to_basic_block()._post(pmt.intern("in"), srcpdu)
        while time.time() - t < 2:  # timeout
            if sink.num_messages() > 0:
                break  # got msg
            time.sleep(0.1)
        tb.stop()
        tb.wait()

        # check data
        recpdu = sink.get_message(0)
        #print 'expected:', pmt.u8vector_elements(pmt.cdr(outpdu))
        #print 'actual  :', pmt.u8vector_elements(pmt.cdr(recpdu))
        self.assertTrue(pmt.equal(recpdu, outpdu))


if __name__ == '__main__':
    gr_unittest.run(qa_rftap_encap)
