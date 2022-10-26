#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

try:
    from gnuradio import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import sandia_utils

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt
import time


class qa_constants (gr_unittest.TestCase):

    def setUp(self):
        self.tb = None

    def tearDown(self):
        self.tb = None

    def test_interned_string_constants(self):

        assert(pmt.eq(sandia_utils.PMTCONSTSTR__time(), pmt.intern("time")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__rx_time(), pmt.intern("rx_time")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__tx_time(), pmt.intern("tx_time")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__direction(), pmt.intern("direction")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__RX(), pmt.intern("RX")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__TX(), pmt.intern("TX")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__lo_freq(), pmt.intern("lo_freq")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__rate(), pmt.intern("rate")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__bandwidth(), pmt.intern("bandwidth")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__rx_rate(), pmt.intern("rx_rate")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__rx_freq(), pmt.intern("rx_freq")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__sob(), pmt.intern("sob")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__eob(), pmt.intern("eob")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__pdu(), pmt.intern("pdu")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__fname(), pmt.intern("fname")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__in(), pmt.intern("in")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__out(), pmt.intern("out")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__tune(), pmt.intern("tune")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__overflow(), pmt.intern("overflow")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__BLOCK(), pmt.intern("BLOCK")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__pdu_in(), pmt.intern("pdu_in")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__pdu_out(), pmt.intern("pdu_out")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__energy(), pmt.intern("energy")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__power(), pmt.intern("power")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__dsp_freq(), pmt.intern("dsp_freq")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__burst_time(), pmt.intern("burst_time")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__tagged_bits_block(), pmt.intern("tagged_bits_block")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__samples(), pmt.intern("samples")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__item_size(), pmt.intern("item_size")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__tune_request(), pmt.intern("tune_request")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__publish(), pmt.intern("publish")))
        assert(pmt.eq(sandia_utils.PMTCONSTSTR__timeout(), pmt.intern("timeout")))


if __name__ == '__main__':
    gr_unittest.run(qa_constants)
