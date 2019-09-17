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
