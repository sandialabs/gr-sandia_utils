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
import numpy as np
from gnuradio import pdu_utils
import os
import time
import pmt
import tempfile
import struct
try:
    from gnuradio import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import sandia_utils


class qa_message_file_debug(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiate(self):
        with tempfile.NamedTemporaryFile() as temp:
            self.emitter = pdu_utils.message_emitter()
            self.dut = sandia_utils.message_file_debug(temp.name)

            self.tb.msg_connect((self.emitter, 'msg'), (self.dut, 'in'))

            # run flowgraph
            self.tb.start()
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait() # don't wait...may not return in time

            self.assertTrue(os.path.exists(temp.name))

    def test_002_dict(self):
        with tempfile.NamedTemporaryFile() as temp:
            self.emitter = pdu_utils.message_emitter()
            self.dut = sandia_utils.message_file_debug(temp.name)

            # single-element dictionary
            p = pmt.dict_add(pmt.make_dict(), pmt.intern('key'), pmt.intern('value'))

            # expected output - blocks add a newline character
            expected = repr(p).encode('utf-8') + '\n'.encode('utf-8')

            # generate flowgraph
            self.tb.msg_connect((self.emitter, 'msg'), (self.dut, 'in'))

            # run flowgraph
            self.tb.start()
            time.sleep(.1)
            self.emitter.emit(p)
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait() # don't wait...may not return in time

            # read file
            self.assertTrue(os.path.exists(temp.name))
            with open(temp.name, 'r') as f:
                actual = f.read()

            print(f"expected: {expected}")
            print(f"actual: {actual.encode('utf-8')}")
            self.assertTrue(actual.encode('utf-8') == expected)

    def test_003_invalid_ascii(self):
        with tempfile.NamedTemporaryFile() as temp:
            self.emitter = pdu_utils.message_emitter()
            self.dut = sandia_utils.message_file_debug(temp.name)

            # pdu with uint8 data
            p = pmt.init_u8vector(1, [255])

            # expected value is the string '#[255]\n'
            expected = bytearray([35, 91, 50, 53, 53, 93, 10])

            # generate flowgraph
            self.tb.msg_connect((self.emitter, 'msg'), (self.dut, 'in'))

            # run flowgraph
            self.tb.start()
            time.sleep(.1)
            self.emitter.emit(p)
            time.sleep(.1)
            self.tb.stop()
            self.tb.wait() # don't wait...may not return in time

            # read file
            self.assertTrue(os.path.exists(temp.name))
            with open(temp.name, 'rb') as f:
                actual = f.read()

            print(f"expected: {expected}")
            print(f"actual: {actual}")
            self.assertTrue(actual == expected)


if __name__ == '__main__':
    gr_unittest.run(qa_message_file_debug)
