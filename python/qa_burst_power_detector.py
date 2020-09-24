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
import time


class qa_burst_power_detector (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiation(self):
        # data
        src_data = (1 + 1j, 2 + 2j, 3 + 3j)
        expected_result = ()

        # blocks
        src = blocks.vector_source_c(src_data)
        dut = sandia_utils.burst_power_detector(150, 10, 10, 1000)
        dst = blocks.vector_sink_c()
        pst = blocks.vector_sink_f()
        self.tb.connect((src, 0), (dut, 0))
        self.tb.connect((dut, 0), (dst, 0))
        self.tb.connect((dut, 1), (pst, 0))

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertEqual(expected_result, result_data)


if __name__ == '__main__':
    gr_unittest.run(qa_burst_power_detector, "qa_burst_power_detector.xml")
