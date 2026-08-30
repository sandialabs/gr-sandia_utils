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

import json
import os
import shutil
import tempfile
import time
import numpy as np
import pmt


class qa_sigmf(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.dirname = tempfile.mkdtemp()

        # recording parameters
        self.rate = 1000000
        self.freq = 915000000
        self.epoch_sec = 1735689600     # 2025-01-01T00:00:00Z
        self.epoch_frac = 0.25
        self.nsamples = 4096
        self.data = (np.arange(self.nsamples) +
                     1j * np.arange(self.nsamples)).astype(np.complex64)

    def tearDown(self):
        self.tb = None
        shutil.rmtree(self.dirname, ignore_errors=True)

    def source(self, key, value):
        return gr.tag_utils.python_to_tag(
            (0, pmt.intern(key), value, pmt.intern("qa_sigmf")))

    def record(self, name_spec):
        """ write a sigmf recording of self.data and return the base name """
        tags = [self.source('rx_freq', pmt.from_double(self.freq)),
                self.source('rx_rate', pmt.from_double(self.rate)),
                self.source('rx_time', pmt.make_tuple(
                    pmt.from_uint64(self.epoch_sec),
                    pmt.from_double(self.epoch_frac)))]

        src = blocks.vector_source_c(self.data.tolist(), False, 1, tags)
        sink = sandia_utils.file_sink('complex', gr.sizeof_gr_complex, 'sigmf',
                                      sandia_utils.MANUAL, 0, self.rate,
                                      self.dirname, name_spec)
        sink.set_second_align(False)
        sink.set_gen_new_folder(False)
        sink.set_recording(True)

        self.tb.connect(src, sink)
        self.tb.run()
        self.tb = None

        return os.path.join(self.dirname, os.path.splitext(name_spec)[0])

    def playback(self, filename):
        """ play a sigmf recording back and return the samples and tags """
        source = sandia_utils.file_source(gr.sizeof_gr_complex, filename,
                                          'sigmf', False, True)
        source.set_begin_tag(pmt.PMT_NIL)
        sink = blocks.vector_sink_c()

        tb = gr.top_block()
        tb.connect(source, sink)
        tb.start()
        time.sleep(0.5)
        tb.stop()
        tb.wait()

        tags = dict((pmt.symbol_to_string(tag.key), tag.value)
                    for tag in sink.tags())

        return np.array(sink.data()), tags

    def test_001_instantiation(self):
        source = sandia_utils.file_source(gr.sizeof_gr_complex, '', 'sigmf',
                                          False, False)
        self.assertEqual('file_source', source.name())

        sink = sandia_utils.file_sink('complex', gr.sizeof_gr_complex, 'sigmf',
                                      sandia_utils.MANUAL, 0, self.rate,
                                      self.dirname, 'test.dat')
        self.assertEqual('file_sink', sink.name())

    def test_002_metadata_written(self):
        """ the sink writes the sample and metadata files as a pair """
        base = self.record('test.dat')

        self.assertTrue(os.path.exists(base + '.sigmf-data'))
        self.assertTrue(os.path.exists(base + '.sigmf-meta'))
        self.assertEqual(self.nsamples * gr.sizeof_gr_complex,
                         os.path.getsize(base + '.sigmf-data'))

        with open(base + '.sigmf-meta') as f:
            meta = json.load(f)

        self.assertEqual('cf32_le', meta['global']['core:datatype'])
        self.assertEqual(self.rate, meta['global']['core:sample_rate'])
        self.assertEqual('1.0.0', meta['global']['core:version'])

        capture = meta['captures'][0]
        self.assertEqual(0, capture['core:sample_start'])
        self.assertEqual(self.freq, capture['core:frequency'])
        self.assertEqual('2025-01-01T00:00:00.250000Z', capture['core:datetime'])

    def test_003_tags_on_playback(self):
        """ frequency and time are recovered from the metadata file """
        base = self.record('test.dat')
        data, tags = self.playback(base + '.sigmf-meta')

        self.assertComplexTuplesAlmostEqual(self.data.tolist(), data.tolist())

        self.assertTrue('rx_freq' in tags)
        self.assertEqual(self.freq, pmt.to_double(tags['rx_freq']))

        # sample rate is tagged under both keys
        self.assertTrue('rate' in tags)
        self.assertEqual(self.rate, pmt.to_double(tags['rate']))

        self.assertTrue('rx_rate' in tags)
        self.assertEqual(self.rate, pmt.to_double(tags['rx_rate']))

        self.assertTrue('rx_time' in tags)
        self.assertEqual(self.epoch_sec,
                         pmt.to_uint64(pmt.tuple_ref(tags['rx_time'], 0)))
        self.assertAlmostEqual(self.epoch_frac,
                               pmt.to_double(pmt.tuple_ref(tags['rx_time'], 1)))

    def test_004_open_by_any_name(self):
        """ either member of the pair, or the base name, may be opened """
        base = self.record('test.dat')

        for filename in [base, base + '.sigmf-data', base + '.sigmf-meta']:
            data, tags = self.playback(filename)
            self.assertEqual(self.nsamples, len(data))
            self.assertEqual(self.freq, pmt.to_double(tags['rx_freq']))

    def test_006_file_sequence(self):
        """ every file of a sequence is tagged when file tags are enabled """
        per_file = 1024
        nfiles = self.nsamples // per_file

        # record a sequence of files, collecting the sink completion messages
        tags = [self.source('rx_freq', pmt.from_double(self.freq)),
                self.source('rx_rate', pmt.from_double(self.rate)),
                self.source('rx_time', pmt.make_tuple(
                    pmt.from_uint64(self.epoch_sec),
                    pmt.from_double(self.epoch_frac)))]

        src = blocks.vector_source_c(self.data.tolist(), False, 1, tags)
        sink = sandia_utils.file_sink('complex', gr.sizeof_gr_complex, 'sigmf',
                                      sandia_utils.MANUAL, per_file, self.rate,
                                      self.dirname, 'seq_%03fd.dat')
        sink.set_second_align(False)
        sink.set_gen_new_folder(False)
        sink.set_recording(True)

        debug = blocks.message_debug()
        self.tb.connect(src, sink)
        self.tb.msg_connect(sink, 'pdu', debug, 'store')
        self.tb.run()
        self.tb = None

        # the sink names the sample file of each completed pair, and the
        # metadata file is complete before the update is published
        self.assertEqual(nfiles, debug.num_messages())
        filenames = []
        for i in range(debug.num_messages()):
            meta = pmt.car(debug.get_message(i))
            fname = pmt.symbol_to_string(
                pmt.dict_ref(meta, pmt.intern('fname'), pmt.PMT_NIL))
            self.assertTrue(fname.endswith('.sigmf-data'))
            self.assertTrue(
                os.path.exists(os.path.splitext(fname)[0] + '.sigmf-meta'))
            filenames.append(fname)

        # play the whole sequence back through a single source
        source = sandia_utils.file_source(gr.sizeof_gr_complex, '', 'sigmf',
                                          False, False)
        source.set_begin_tag(pmt.PMT_NIL)
        source.add_file_tags(True)
        source.set_file_queue_depth(100)

        sink = blocks.vector_sink_c()
        tb = gr.top_block()
        tb.connect(source, sink)
        tb.start()

        # the files must be handed over while the flowgraph runs, as a file
        # monitor would do.  each one is drained before the next is queued so
        # that the tagging of each file is independent of the previous one
        for i, fname in enumerate(filenames):
            source.open(fname, False)
            deadline = time.time() + 5.0
            while (len(sink.data()) < (i + 1) * per_file
                   and time.time() < deadline):
                time.sleep(0.01)

        tb.stop()
        tb.wait()

        self.assertComplexTuplesAlmostEqual(self.data.tolist(), sink.data())

        # one tag set at the first sample of every file
        tagged = {}
        for tag in sink.tags():
            tagged.setdefault(tag.offset, {})[
                pmt.symbol_to_string(tag.key)] = tag.value

        self.assertEqual([i * per_file for i in range(nfiles)], sorted(tagged))

        for i, offset in enumerate(sorted(tagged)):
            values = tagged[offset]
            self.assertEqual(self.freq, pmt.to_double(values['rx_freq']))
            self.assertEqual(self.rate, pmt.to_double(values['rx_rate']))

            # each file starts one file length after the previous one.  the
            # datetime field has microsecond resolution, so allow for the
            # truncation of the fractional second
            expected = (self.epoch_sec + self.epoch_frac +
                        i * per_file / float(self.rate))
            actual = (pmt.to_uint64(pmt.tuple_ref(values['rx_time'], 0)) +
                      pmt.to_double(pmt.tuple_ref(values['rx_time'], 1)))
            self.assertAlmostEqual(expected, actual, delta=2e-6)

    def test_005_missing_metadata(self):
        """ samples remain readable when the metadata file is absent """
        base = self.record('test.dat')
        os.remove(base + '.sigmf-meta')

        data, tags = self.playback(base + '.sigmf-data')
        self.assertEqual(self.nsamples, len(data))
        self.assertEqual(0, len(tags))


if __name__ == '__main__':
    gr_unittest.run(qa_sigmf)
