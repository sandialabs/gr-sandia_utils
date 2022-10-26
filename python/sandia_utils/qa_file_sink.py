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

from gnuradio import pdu_utils
import time


class qa_file_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.emitter = pdu_utils.message_emitter()
        self.debug = blocks.message_debug()

    def tearDown(self):
        self.tb = None

    def test_001_instantiation(self):

        sink = sandia_utils.file_sink("message", gr.sizeof_gr_complex * 1, 'message', sandia_utils.MANUAL, 0, int(1), "/dev/", "null")
        sink.set_recording(False)
        sink.set_gen_new_folder(True)
        sink.set_second_align(True)
        sink.set_file_num_rollover(0)

        self.tb.msg_connect((self.emitter, 'msg'), (sink, 'in'))

        self.tb.start()
        time.sleep(.5)
        self.tb.stop()
        self.tb.wait()

        self.assertTrue(True)


if __name__ == '__main__':
    gr_unittest.run(qa_file_sink)
