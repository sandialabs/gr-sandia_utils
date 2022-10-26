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


class qa_stream_gate(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_flow(self):
        # data
        src_data = (1, 1, 2, 2, 3, 3)
        expected_result = [1.0, 1.0, 2.0, 2.0, 3.0, 3.0]

        # blocks
        src = blocks.vector_source_f(src_data)
        cts = sandia_utils.stream_gate_f(True, True)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, cts)
        self.tb.connect(cts, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(expected_result, result_data)

    def test_002_block(self):
        # data
        src_data = (1, 1, 2, 2, 3, 3)
        expected_result = []

        # blocks
        src = blocks.vector_source_f(src_data)
        cts = sandia_utils.stream_gate_f(False, True)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, cts)
        self.tb.connect(cts, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(expected_result, result_data)

    def test_003_flow_vectors(self):
        # data
        src_data = (1, 1, 2, 2, 3, 3)
        expected_result = [1.0, 1.0, 2.0, 2.0, 3.0, 3.0]

        # blocks
        src = blocks.vector_source_s(src_data, False, 2, [])
        cts = sandia_utils.stream_gate_s(True, True, vlen=2)
        dst = blocks.vector_sink_s(2, len(src_data))
        self.tb.connect(src, cts)
        self.tb.connect(cts, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(expected_result, result_data)

    def test_004_all_types(self):
        ''' Ensure all stream gate types can be instantiated '''
        a = sandia_utils.stream_gate_b(True, True, vlen=1)
        b = sandia_utils.stream_gate_s(True, True, vlen=1)
        c = sandia_utils.stream_gate_i(True, True, vlen=1)
        d = sandia_utils.stream_gate_f(True, True, vlen=1)
        e = sandia_utils.stream_gate_c(True, True, vlen=1)


if __name__ == '__main__':
    gr_unittest.run(qa_stream_gate)
