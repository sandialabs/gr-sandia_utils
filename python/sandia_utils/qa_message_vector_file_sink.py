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

import numpy as np
from gnuradio import pdu_utils
import os
import time
import pmt
import tempfile
import struct


class qa_message_vector_file_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):
        with tempfile.NamedTemporaryFile() as temp:
            self.emitter = pdu_utils.message_emitter()
            self.dut = sandia_utils.message_vector_file_sink(temp.name)

            self.tb.msg_connect((self.emitter, 'msg'), (self.dut, 'in'))

            # run flowgraph
            self.tb.start()
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait() # don't wait...may not return in time

            self.assertTrue(True)

    def test_002_single_message(self):
        with tempfile.NamedTemporaryFile() as temp:
            emitter = pdu_utils.message_emitter()
            sink = sandia_utils.message_vector_file_sink(temp.name, append=False)

            self.tb.msg_connect((emitter, 'msg'), (sink, 'in'))

            # generate pdu
            v = np.array([48, 49, 50], dtype='uint8')
            p = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(v), v.tolist()))

            # run flowgraph
            self.tb.start()
            time.sleep(.01)
            emitter.emit(p)
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait()

            # validate file
            self.assertTrue(os.path.exists(temp.name))
            vals = np.fromfile(temp.name, dtype='uint8')
            self.assertTrue((v == vals).all())

    def test_003_multi_message(self):
        with tempfile.NamedTemporaryFile() as temp:
            emitter = pdu_utils.message_emitter()
            sink = sandia_utils.message_vector_file_sink(temp.name, append=False)

            self.tb.msg_connect((emitter, 'msg'), (sink, 'in'))

            # generate pdu
            v1 = np.array([48, 49, 50], dtype='uint8')
            v2 = np.array([50, 51, 52], dtype='uint8')
            p1 = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(v1), v1.tolist()))
            p2 = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(v2), v2.tolist()))

            # run flowgraph
            self.tb.start()
            time.sleep(.01)
            emitter.emit(p1)
            emitter.emit(p2)
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait()

            # validate file
            self.assertTrue(os.path.exists(temp.name))
            vals = np.fromfile(temp.name, dtype='uint8')
            self.assertTrue((v2 == vals).all())

    def test_004_multi_message(self):
        with tempfile.NamedTemporaryFile() as temp:
            emitter = pdu_utils.message_emitter()
            sink = sandia_utils.message_vector_file_sink(temp.name, append=True)

            self.tb.msg_connect((emitter, 'msg'), (sink, 'in'))

            # generate pdu
            v = [[48, 49, 50], [51, 52]]
            p1 = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(v[0]), v[0]))
            p2 = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(v[1]), v[1]))

            # run flowgraph
            self.tb.start()
            time.sleep(.01)
            emitter.emit(p1)
            emitter.emit(p2)
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait()

            # validate file
            self.assertTrue(os.path.exists(temp.name))
            ind = 0
            with open(temp.name, 'rb') as f:
                nbytes = f.read(4)
                while nbytes != b'':
                    n = struct.unpack('I', nbytes)[0]
                    vals = list(f.read(n))
                    self.assertTrue(v[ind] == vals)

                    # move to next vector
                    ind += 1
                    nbytes = f.read(4)

            # should have processed two vectors
            self.assertTrue(ind == 2)


if __name__ == '__main__':
    gr_unittest.run(qa_message_vector_file_sink)
