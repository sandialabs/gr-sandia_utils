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
    import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    import sandia_utils
import pmt


class qa_tag_debug_file(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_notags(self):
        # data
        src_data = (1, 1, 2, 2, 3, 3)

        # blocks
        src = blocks.vector_source_f(src_data)
        cts = sandia_utils.tag_debug_file(gr.sizeof_float * 1, '', "", "/dev/null")

        self.tb.connect(src, cts)

        # execute
        self.tb.run()

    def test_002_tags(self):
        # data
        src_data = (1, 1, 2, 2, 3, 3)

        # blocks
        sob_tag = gr.tag_utils.python_to_tag((34, pmt.intern("SOB"), pmt.PMT_T, pmt.intern("src")))
        eob_tag = gr.tag_utils.python_to_tag((34 + (8 * 31), pmt.intern("EOB"), pmt.PMT_T, pmt.intern("src")))
        src = blocks.vector_source_f(range(350), False, 1, [sob_tag, eob_tag])

        cts = sandia_utils.tag_debug_file(gr.sizeof_float * 1, '', "", "/dev/null")

        self.tb.connect(src, cts)

        # execute
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(qa_tag_debug_file)
