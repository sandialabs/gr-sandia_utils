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
from gnuradio import pdu_utils
import pmt
import time
import os
from pathlib import Path
try:
    from gnuradio import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import sandia_utils


class qa_file_archiver(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test1_copy_file(self):
        # open a dummy file
        fname = '/tmp/foo.txt'
        fname_out = '/tmp/915_30_000000.txt'
        if os.path.exists(fname):
            os.remove(fname)
        Path(fname).touch()

        if os.path.exists(fname_out):
            os.remove(fname_out)

        # PMT
        p = pmt.dict_add(pmt.make_dict(), pmt.intern('rx_freq'), pmt.from_double(915e6))
        p = pmt.dict_add(p, pmt.intern('rx_rate'), pmt.from_double(30.72e6))
        p = pmt.dict_add(p, pmt.intern('rx_time'), pmt.make_tuple(
            pmt.from_uint64(0), pmt.from_double(0)))
        p = pmt.dict_add(p, pmt.intern('fname'), pmt.intern(fname))

        # blocks
        emitter = pdu_utils.message_emitter(pmt.PMT_NIL)
        archiver = sandia_utils.file_archiver('/tmp', '%fcM_%fsM_%H%M%S.txt',
                                              True)

        # connect
        self.tb.msg_connect((emitter, 'msg'), (archiver, 'pdu'))

        # run
        self.tb.start()
        emitter.emit(p)
        time.sleep(.1)
        # clean up
        self.tb.stop()
        self.tb.wait()

        # check data
        os.remove(fname)
        self.assertTrue(os.path.exists(fname_out))

        # cleanup
        os.remove(fname_out)


if __name__ == '__main__':
    gr_unittest.run(qa_file_archiver)
