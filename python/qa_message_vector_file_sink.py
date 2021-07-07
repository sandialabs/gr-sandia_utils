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
import pdu_utils
import time


class qa_message_vector_file_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

        self.emitter = pdu_utils.message_emitter()
        self.dut = sandia_utils.message_vector_file_sink('/tmp/qa_message_vector_file_sink')

        self.tb.msg_connect((self.emitter, 'msg'), (self.dut, 'in'))

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):

        # run flowgraph
        self.tb.start()
        time.sleep(.1)
        self.tb.stop()
        self.tb.wait() # don't wait...may not return in time

        self.assertTrue(True)


if __name__ == '__main__':
    gr_unittest.run(qa_message_vector_file_sink)
