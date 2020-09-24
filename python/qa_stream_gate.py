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


class qa_stream_gate(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_flow(self):
        # data
        src_data = (1, 1, 2, 2, 3, 3)
        expected_result = (1, 1, 2, 2, 3, 3)

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
        expected_result = ()

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


if __name__ == '__main__':
    gr_unittest.run(qa_stream_gate)
