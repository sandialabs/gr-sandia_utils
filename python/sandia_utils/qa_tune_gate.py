#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2022 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
from tune_gate import tune_gate
import numpy as np
import pmt
import time
import sys
try:
    from gnuradio import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import sandia_utils


class qa_tune_gate(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    # simple timeout test (short)
    def test_001_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 50
        n_discard = 10
        n_publish = 30
        samp_rate = 512000
        input_size = 2048

        dtype = np.uint16
        data = [np.ushort(n) for n in range(input_size)]
        out_exp = [n for n in data[n_timeout:(n_timeout + n_publish)]]

        self.source = blocks.vector_source_s(data, False, 1, [])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_s()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_short * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.run()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(list(result), out_exp)
        self.assertEqual(pmt.symbol_to_string(testtag.key), "timeout")
        self.assertEqual(pmt.to_long(testtag.value), n_timeout)

    # overlap timeout test (float)
    def test_002_t(self):
        self.tearDown()
        self.setUp()
        n_discard = 10
        n_publish = 30
        samp_rate = 512000
        input_size = 2048
        n_timeout = input_size - 10

        dtype = np.float32
        data = [float(n) for n in range(1000000)]
        out_exp = data[n_timeout:(n_timeout + n_publish)]

        self.source = blocks.vector_source_f(data, False, 1, [])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_f()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_float * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.run()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertFloatTuplesAlmostEqual(result, out_exp, 10)
        self.assertEqual(pmt.symbol_to_string(testtag.key), "timeout")
        self.assertEqual(pmt.to_long(testtag.value), n_timeout)

    # simple publish tag test (byte)
    def test_003_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 10000
        n_discard = 10
        n_publish = 30
        samp_rate = 512000
        input_size = 4096
        offset = 50

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()

        pub_tag = gr.tag_utils.python_to_tag((offset, pub_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.uint8
        data = [np.uint8(n) for n in range(100)]
        out_exp = [n for n in data[offset:(offset + n_publish)]]

        self.source = blocks.vector_source_b(data, True, 1, [pub_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_b()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_char * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(list(result), out_exp)
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)

    # overlap publish tag test (complex)
    def test_004_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 10000
        n_discard = 10
        n_publish = 30
        samp_rate = 512000
        input_size = 4096
        offset = 3066

        pub_key = sandia_utils.PMTCONSTSTR__publish()

        pub_tag = gr.tag_utils.python_to_tag((offset, pub_key, pmt.PMT_T, pmt.intern("src")))
        dtype = np.complex64
        data = [complex(n) for n in range(1024)]
        out_exp = data[1018:] + data[:(n_publish - (input_size - offset))]

        self.source = blocks.vector_source_c(data, True, 1, [pub_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_c()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_gr_complex * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = list(self.data_debug.data()[:n_publish])
        self.assertComplexTuplesAlmostEqual(result, out_exp, 10)
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)

    # simple tune tag test (int)
    def test_005_t(self):
        n_timeout = 10000
        n_discard = 10
        n_publish = 30
        samp_rate = 512000
        input_size = 4096
        offset = 50

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()

        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(1024)]
        out_exp = [n for n in data[(offset + n_discard):(offset + n_discard + n_publish)]]

        self.source = blocks.vector_source_i(data, False, 1, [tune_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()

        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))
        self.tb.run()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)
        self.assertEqual(list(result), out_exp)

    # overlap tune tag test - publish
    def test_006_t(self):
        n_timeout = 10000
        n_discard = 10
        n_publish = 30
        samp_rate = 512000
        input_size = 1024
        offset = 776

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()

        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(400)]
        out_exp = [n for n in data[386:] + data[:16]]

        self.source = blocks.vector_source_i(data, True, 1, [tune_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()

        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)
        self.assertEqual(list(result), out_exp)

    # overlap tune tag test - discard
    def test_007_t(self):
        n_timeout = 10000
        n_discard = 50
        n_publish = 30
        samp_rate = 512000
        input_size = 1024
        offset = 776

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()

        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(400)]
        out_exp = [n for n in data[26:56]]

        self.source = blocks.vector_source_i(data, True, 1, [tune_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)
        self.assertEqual(list(result), out_exp)

    # publish over multiple work calls
    def test_008_t(self):
        n_timeout = 100000
        n_discard = 50
        n_publish = 3000
        samp_rate = 512000
        input_size = 1024
        offset = 50

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(100)]
        out_exp = data * 30

        self.source = blocks.vector_source_i(data, True, 1, [tune_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))

        self.tb.start()
        time.sleep(0.2)
        self.tb.stop()
        self.tb.wait()

        result = self.data_debug.data()[:n_publish]
        self.assertEqual(list(result), out_exp)

    # discard over multiple work calls
    def test_009_t(self):
        n_timeout = 100000
        n_discard = 3000
        n_publish = 50
        samp_rate = 512000
        input_size = 1024
        offset = 50

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(100)]
        out_exp = data[offset:(n_publish + offset)]

        self.source = blocks.vector_source_i(data, True, 1, [tune_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()

        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)
        self.assertEqual(list(result), out_exp)

    # timeout over multiple work calls
    def test_010_t(self):
        n_timeout = 100000
        n_discard = 50
        n_publish = 50
        samp_rate = 512000
        input_size = 1024

        dtype = np.int32
        data = [int(n) for n in range(100)]
        out_exp = data[:n_publish]

        self.source = blocks.vector_source_i(data, True, 1, [])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.4)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), "timeout")
        self.assertEqual(pmt.to_long(testtag.value), n_timeout)
        self.assertEqual(list(result), out_exp)

    # multiple tag handling
    def test_011_t(self):
        data = [n for n in range(100)]
        out_exp = [float(n) for n in [13, 14, 15, 16, 17]] + [float(n) for n in [30, 31, 32, 33, 34]] + [float(n)
                                                                                                         for n in [40, 41, 42, 43, 44]] + [float(n) for n in [61, 62, 63, 64, 65]] + [float(n) for n in [84, 85, 86, 87, 88]]
        n_timeout = 2000
        n_discard = 10
        n_publish = 5
        dtype = np.int32
        samp_rate = 32000

        tune_offset = [3, 20, 41, 51, 60]
        pub_offset = [4, 25, 40, 84, 88]

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()
        tags = []

        for n in range(len(tune_offset)):
            tags.append(gr.tag_utils.python_to_tag((tune_offset[n], tune_key, pmt.PMT_T, pmt.intern("src"))))
        for n in range(len(pub_offset)):
            tags.append(gr.tag_utils.python_to_tag((pub_offset[n], pub_key, pmt.PMT_T, pmt.intern("src"))))

        self.source = blocks.vector_source_f(data, False, 1, tags)
        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_f()

        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.run()

        testtags = self.tag_debug.current_tags()
        result = self.data_debug.data()
        self.assertEqual(list(result), out_exp)

        self.assertEqual(pmt.symbol_to_string(testtags[0].key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtags[0].value), tune_offset[0])

        self.assertEqual(pmt.symbol_to_string(testtags[1].key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtags[1].value), tune_offset[1])

        self.assertEqual(pmt.symbol_to_string(testtags[2].key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(testtags[2].value), pub_offset[2])

        self.assertEqual(pmt.symbol_to_string(testtags[3].key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(testtags[3].value), tune_offset[3])

        self.assertEqual(pmt.symbol_to_string(testtags[4].key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(testtags[4].value), pub_offset[3])

    # tag during timeout publish test
    def test_012_t(self):
        n_timeout = 100
        n_discard = 10
        n_publish = 50
        samp_rate = 512000
        input_size = 1024
        offset = 120

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()

        pub_tag = gr.tag_utils.python_to_tag((offset, pub_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(100)]
        out_exp = data[20:70]

        self.source = blocks.vector_source_i(data, True, 1, [pub_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.02)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)
        self.assertEqual(list(result), out_exp)

    # tag during timeout overlap publish test
    def test_013_t(self):
        n_timeout = 999
        n_discard = 10
        n_publish = 50
        samp_rate = 512000
        input_size = 1024
        offset = 1030

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        pub_key = sandia_utils.PMTCONSTSTR__publish()

        pub_tag = gr.tag_utils.python_to_tag((offset, pub_key, pmt.PMT_T, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(100)]
        out_exp = data[30:80]

        self.source = blocks.vector_source_i(data, True, 1, [pub_tag])
        self.source.set_min_output_buffer(input_size)
        self.source.set_max_output_buffer(input_size)

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        testtag = self.tag_debug.current_tags()[0]
        result = self.data_debug.data()[:n_publish]
        self.assertEqual(pmt.symbol_to_string(testtag.key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(testtag.value), offset)
        self.assertEqual(list(result), out_exp)

    # tag propagation test
    def test_014_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 100000
        n_discard = 30
        n_publish = 30
        samp_rate = 512000
        input_size = 4096
        offset = 10
        offset_2 = 150

        pub_key = sandia_utils.PMTCONSTSTR__publish()
        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        test_key = pmt.intern("TEST")
        bad_key = pmt.intern("BAD")
        rx_time_key = sandia_utils.PMTCONSTSTR__rx_time()

        current_time = 1.0
        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))
        bad_tag = gr.tag_utils.python_to_tag((offset + 1, bad_key, pmt.PMT_T, pmt.intern("src")))
        test_tag = gr.tag_utils.python_to_tag((offset + n_discard + 1, test_key, pmt.PMT_T, pmt.intern("src")))
        rx_time_tag = gr.tag_utils.python_to_tag((offset + n_discard + 2, rx_time_key, pmt.from_double(current_time), pmt.intern("src")))

        pub_tag_2 = gr.tag_utils.python_to_tag((offset_2, pub_key, pmt.PMT_T, pmt.intern("src")))
        test_tag_2 = gr.tag_utils.python_to_tag((offset_2 + 1, test_key, pmt.PMT_T, pmt.intern("src")))
        rx_time_tag_2 = gr.tag_utils.python_to_tag((offset_2 + 2, rx_time_key, pmt.from_double(current_time), pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(10000)]
        out_exp = [n for n in data[(offset + n_discard):(offset + n_publish + n_discard)] + data[offset_2:offset_2 + n_publish]]

        self.source = blocks.vector_source_i(data, False, 1, [tune_tag, bad_tag, test_tag, rx_time_tag, pub_tag_2, test_tag_2, rx_time_tag_2])

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.run()

        out_tags = self.tag_debug.current_tags()
        out_tune_tag = out_tags[0]
        out_test_tag = out_tags[1]
        out_time_tag = out_tags[2]
        out_pub_tag_2 = out_tags[3]
        out_test_tag_2 = out_tags[4]
        out_time_tag_2 = out_tags[5]
        result = self.data_debug.data()[:(2 * n_publish)]
        self.assertEqual(list(result), out_exp)
        self.assertEqual(pmt.symbol_to_string(out_tune_tag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(out_tune_tag.value), offset)
        self.assertEqual(pmt.symbol_to_string(out_test_tag.key), pmt.symbol_to_string(test_key))
        self.assertEqual(out_test_tag.offset, 1)
        self.assertEqual(out_time_tag.offset, 2)
        self.assertEqual(pmt.symbol_to_string(out_time_tag.key), pmt.symbol_to_string(rx_time_key))
        self.assertAlmostEqual(pmt.to_double(out_time_tag.value) - current_time, n_discard / samp_rate, 6)

        self.assertEqual(pmt.symbol_to_string(out_pub_tag_2.key), pmt.symbol_to_string(pub_key))
        self.assertEqual(pmt.to_long(out_pub_tag_2.value), offset_2)
        self.assertEqual(pmt.symbol_to_string(out_test_tag_2.key), pmt.symbol_to_string(test_key))
        self.assertEqual(out_test_tag_2.offset, n_publish + 1)
        self.assertEqual(out_time_tag_2.offset, n_publish + 2)
        self.assertEqual(pmt.symbol_to_string(out_time_tag_2.key), pmt.symbol_to_string(rx_time_key))
        self.assertAlmostEqual(pmt.to_double(out_time_tag_2.value) - current_time, n_discard / samp_rate, 6)

    # rx_rate update test
    def test_015_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 100000
        n_discard = 100
        n_publish = 100
        samp_rate_0 = 512000
        samp_rate_1 = 256000
        samp_rate_2 = 128000
        input_size = 4096
        offset = 100
        offset_2 = 500

        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        rx_time_key = sandia_utils.PMTCONSTSTR__rx_time()
        rx_rate_key = sandia_utils.PMTCONSTSTR__rx_rate()

        current_time = 1.0
        rx_rate_tag_1 = gr.tag_utils.python_to_tag((5, rx_rate_key, pmt.from_double(samp_rate_1), pmt.intern("src")))
        rx_rate_tag_2 = gr.tag_utils.python_to_tag((offset_2 + 50, rx_rate_key, pmt.from_double(samp_rate_2), pmt.intern("src")))
        tune_tag_1 = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))
        tune_tag_2 = gr.tag_utils.python_to_tag((offset_2, tune_key, pmt.PMT_T, pmt.intern("src")))
        rx_time_tag_1 = gr.tag_utils.python_to_tag((offset + 150, rx_time_key, pmt.from_double(current_time), pmt.intern("src")))
        rx_time_tag_2 = gr.tag_utils.python_to_tag((offset_2 + 150, rx_time_key, pmt.from_double(current_time), pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(10000)]
        out_exp = [n for n in data[(offset + n_discard):(offset + n_publish + n_discard)] + data[offset_2:offset_2 + n_publish]]

        self.source = blocks.vector_source_i(data, False, 1, [rx_rate_tag_1, tune_tag_1, rx_time_tag_1, tune_tag_2, rx_rate_tag_2, rx_time_tag_2])

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate_0, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.run()

        out_tags = self.tag_debug.current_tags()
        out_tune_tag = out_tags[0]
        out_time_tag = out_tags[1]
        out_tune_tag_2 = out_tags[2]
        out_time_tag_2 = out_tags[3]

        self.assertEqual(pmt.symbol_to_string(out_tune_tag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(out_tune_tag.value), offset)

        self.assertEqual(pmt.symbol_to_string(out_time_tag.key), pmt.symbol_to_string(rx_time_key))
        self.assertAlmostEqual(pmt.to_double(out_time_tag.value) - current_time, n_discard / samp_rate_1, 6)

        self.assertEqual(pmt.symbol_to_string(out_tune_tag_2.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(out_tune_tag_2.value), offset_2)

        self.assertEqual(pmt.symbol_to_string(out_time_tag_2.key), pmt.symbol_to_string(rx_time_key))
        self.assertAlmostEqual(pmt.to_double(out_time_tag_2.value) - current_time, 1.5 * n_discard / samp_rate_1 + 0.5 * n_discard / samp_rate_2, 6)

    # tag propagation test across work calls
    def test_016_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 100000
        n_discard = 30
        n_publish = 30
        samp_rate = 512000
        input_size = 512
        offset = 10
        offset_2 = 32000

        pub_key = sandia_utils.PMTCONSTSTR__publish()
        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        test_key = pmt.intern("TEST")
        rx_time_key = sandia_utils.PMTCONSTSTR__rx_time()

        current_time = 1.0

        dtype = np.int32
        data = [int(n) for n in range(10000)]
        out_exp = [n for n in data[(offset + n_discard):(offset + n_publish + n_discard)] + data[offset_2:offset_2 + n_publish]]

        tune_tag_1 = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))
        test_tag_1 = gr.tag_utils.python_to_tag((offset + 40, test_key, pmt.PMT_T, pmt.intern("src")))
        tune_tag_2 = gr.tag_utils.python_to_tag((offset_2, tune_key, pmt.PMT_T, pmt.intern("src")))
        test_tag_2 = gr.tag_utils.python_to_tag((offset_2 + 40, test_key, pmt.PMT_T, pmt.intern("src")))

        self.source = blocks.vector_source_i(data, True, 1, [tune_tag_1, test_tag_1, tune_tag_2, test_tag_2])

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.start()
        time.sleep(0.25)
        self.tb.stop()
        self.tb.wait()

        out_tags = self.tag_debug.current_tags()
        out_tune_tag_1 = out_tags[0]
        out_test_tag_1 = out_tags[1]
        out_tune_tag_2 = out_tags[8]
        out_test_tag_2 = out_tags[9]
        self.assertEqual(pmt.symbol_to_string(out_tune_tag_1.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(out_tune_tag_1.value), offset)
        self.assertEqual(out_tune_tag_1.offset, 0)
        self.assertEqual(pmt.symbol_to_string(out_test_tag_1.key), pmt.symbol_to_string(test_key))
        self.assertEqual(out_test_tag_1.offset, 10)

        self.assertEqual(pmt.symbol_to_string(out_tune_tag_2.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(out_tune_tag_2.offset, 4 * n_publish)
        self.assertEqual(pmt.to_long(out_tune_tag_2.value), offset_2)
        self.assertEqual(pmt.symbol_to_string(out_test_tag_2.key), pmt.symbol_to_string(test_key))
        self.assertEqual(out_test_tag_2.offset, 4 * n_publish + 10)

    # rx time update with UHD time

    def test_017_t(self):
        self.tearDown()
        self.setUp()
        n_timeout = 100000
        n_discard = 30
        n_publish = 30
        samp_rate = 512000
        input_size = 4096
        offset = 10
        offset_2 = 150

        pub_key = sandia_utils.PMTCONSTSTR__publish()
        tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        test_key = pmt.intern("TEST")
        bad_key = pmt.intern("BAD")
        rx_time_key = sandia_utils.PMTCONSTSTR__rx_time()

        current_time_sec = 1
        current_time_frac = 0.99997

        rx_time_value = pmt.cons(pmt.from_uint64(current_time_sec), pmt.from_double(current_time_frac))

        tune_tag = gr.tag_utils.python_to_tag((offset, tune_key, pmt.PMT_T, pmt.intern("src")))
        bad_tag = gr.tag_utils.python_to_tag((offset + 1, bad_key, pmt.PMT_T, pmt.intern("src")))
        test_tag = gr.tag_utils.python_to_tag((offset + n_discard + 1, test_key, pmt.PMT_T, pmt.intern("src")))
        rx_time_tag = gr.tag_utils.python_to_tag((offset + n_discard + 2, rx_time_key, rx_time_value, pmt.intern("src")))

        dtype = np.int32
        data = [int(n) for n in range(10000)]
        out_exp = [n for n in data[(offset + n_discard):(offset + n_publish + n_discard)] + data[offset_2:offset_2 + n_publish]]

        self.source = blocks.vector_source_i(data, False, 1, [tune_tag, bad_tag, test_tag, rx_time_tag])

        self.cut = tune_gate(n_publish, n_discard, n_timeout, samp_rate, dtype)
        self.data_debug = blocks.vector_sink_i()
        self.tag_debug = sandia_utils.sandia_tag_debug(gr.sizeof_int * 1, '', "", True)
        self.tag_debug.set_display(False)

        self.tb.connect((self.source, 0), (self.cut, 0))
        self.tb.connect((self.cut, 0), (self.data_debug, 0))
        self.tb.connect((self.cut, 0), (self.tag_debug, 0))

        self.tb.run()

        out_tags = self.tag_debug.current_tags()
        out_tune_tag = out_tags[0]
        out_test_tag = out_tags[1]
        out_time_tag = out_tags[2]

        result = self.data_debug.data()[:n_publish]
        #self.assertEqual(list(result), out_exp)
        self.assertEqual(pmt.symbol_to_string(out_tune_tag.key), pmt.symbol_to_string(tune_key))
        self.assertEqual(pmt.to_long(out_tune_tag.value), offset)
        self.assertEqual(pmt.symbol_to_string(out_test_tag.key), pmt.symbol_to_string(test_key))
        self.assertEqual(out_test_tag.offset, 1)
        self.assertEqual(out_time_tag.offset, 2)
        self.assertEqual(pmt.symbol_to_string(out_time_tag.key), pmt.symbol_to_string(rx_time_key))

        out_rx_time = pmt.to_uint64(pmt.car(out_time_tag.value)) + pmt.to_double(pmt.cdr(out_time_tag.value))
        exp_rx_time = current_time_sec + current_time_frac

        self.assertAlmostEqual(out_rx_time - exp_rx_time, n_discard / samp_rate, 6)


if __name__ == '__main__':
    gr_unittest.run(qa_tune_gate)
