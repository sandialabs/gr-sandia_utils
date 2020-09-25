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

class qa_interleaved_short_to_complex(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_no_scale(self):
        # data
        src_data = (1,1,2,2,3,3)
        expected_result = (1+1j,2+2j,3+3j)

        # blocks
        src = blocks.vector_source_s(src_data)
        stc = sandia_utils.interleaved_short_to_complex(False,False)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, stc)
        self.tb.connect(stc, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

    def test_with_scale(self):
        # data
        src_data = (1,1,2,2,3,3)
        scale = 2.0
        expected_result = tuple(i/scale for i in (1+1j,2+2j,3+3j))

        # blocks
        src = blocks.vector_source_s(src_data)
        stc = sandia_utils.interleaved_short_to_complex(False,False,scale)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, stc)
        self.tb.connect(stc, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

    def test_vector(self):
        # data
        src_data = (1,1,2,2,3,3)
        expected_result = (1+1j,2+2j,3+3j)

        # blocks
        src = blocks.vector_source_s(src_data, vlen=2)
        stc = sandia_utils.interleaved_short_to_complex(True,False)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, stc)
        self.tb.connect(stc, dst)

        # execute
        self.tb.run()
        result_data = dst.data()

        # assert
        print("got {}, expected {}".format(result_data, expected_result))
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(qa_interleaved_short_to_complex, "qa_interleaved_short_to_complex.xml")
