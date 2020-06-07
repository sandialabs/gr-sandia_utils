#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 gr-sandia_utils author.
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
from csv_writer import csv_writer
from csv_reader import csv_reader
import pmt
import pdu_utils
import time


class qa_csv_writer (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def check_file(self, fname, expected, has_header=False, data_type='uint8'):
        # instantiate objects
        debug = blocks.message_debug()
        reader = csv_reader(fname=fname, has_header=has_header,
                            period=10, repeat=False, start_delay=0,
                            data_type=data_type)

        tb = gr.top_block()
        tb.msg_connect((reader, 'out'), (debug, 'store'))
        tb.start()
        time.sleep(.25)
        tb.stop()
        tb.wait()

        # collect results
        got = debug.get_message(0)
        got_car = pmt.car(got)

        print ("got: {}".format(got))
        print ("expected: {}".format(expected))

        # do a manual equal on elements because pmt equal is not the best
        expected_items = pmt.dict_items(pmt.car(expected))
        valid = True
        for i in range(pmt.length(expected_items)):
            key = pmt.car(pmt.nth(i, expected_items))
            value = pmt.cdr(pmt.nth(i, expected_items))
            got_value = pmt.dict_ref(got_car, key, pmt.PMT_NIL)
            if not pmt.equal(got_value, value):
                valid = False
                break

        return (valid and pmt.equal(pmt.cdr(got), pmt.cdr(expected)))

    def test_001_uint8_no_header(self):
        emitter = pdu_utils.message_emitter()
        writer = csv_writer('/tmp/file.csv', False, '', 'uint8')

        # generate pdu
        metadata = pmt.PMT_NIL
        data = pmt.init_u8vector(5, [11, 12, 13, 14, 15])
        expected = pmt.cons(metadata, data)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()
        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, has_header=False))

    def test_002_uint8_metadata_header(self):
        emitter = pdu_utils.message_emitter()
        writer = csv_writer('/tmp/file.csv', True, '', 'uint8')

        # generate pdu
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('a'), pmt.intern('a'))
        metadata = pmt.dict_add(metadata, pmt.intern('b'), pmt.from_long((0)))
        data = pmt.init_u8vector(5, [11, 12, 13, 14, 15])
        pdu = pmt.cons(metadata, data)

        # expected will only have intern strings
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('a'), pmt.intern('a'))
        metadata = pmt.dict_add(metadata, pmt.intern('b'), pmt.intern('0'))
        expected = pmt.cons(metadata, data)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, has_header=True))

    def test_003_uint8_formatted_header(self):
        emitter = pdu_utils.message_emitter()
        writer = csv_writer('/tmp/file.csv', True, 'a(string),b(long)', 'uint8')

        # generate pdu
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('a'), pmt.intern('a'))
        metadata = pmt.dict_add(metadata, pmt.intern('b'), pmt.from_long((0)))
        data = pmt.init_u8vector(5, [11, 12, 13, 14, 15])
        expected = pmt.cons(metadata, data)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, has_header=True))

    def test_004_all_data_types(self):
        # data types and their constructors
        data_types = {'uint8': pmt.init_u8vector,
                      'int8': pmt.init_s8vector,
                      'uint16': pmt.init_u16vector,
                      'int16': pmt.init_s16vector,
                      'uint32': pmt.init_u32vector,
                      'int32': pmt.init_s32vector,
                      'float': pmt.init_f32vector,
                      'complex float': pmt.init_c32vector,
                      'double': pmt.init_f64vector,
                      'complex double': pmt.init_c64vector
                      }

        passed = True
        for data_type, init_func in data_types.items():
            emitter = pdu_utils.message_emitter()
            writer = csv_writer('/tmp/file.csv', False, '', data_type)

            # expected pdu
            data = init_func(5, [1, 2, 3, 4, 5])
            expected = pmt.cons(pmt.PMT_NIL, data)

            # run
            tb = gr.top_block()
            tb.msg_connect((emitter, 'msg'), (writer, 'in'))
            tb.start()
            emitter.emit(expected)
            time.sleep(.5)
            tb.stop()
            tb.wait()

            passed &= self.check_file('/tmp/file.csv', expected, has_header=False,
                                      data_type=data_type)

        self.assertTrue(passed)

    def test_005_all_header_fields(self):
        emitter = pdu_utils.message_emitter()
        fields = 'field0(string),field1(bool),field2(long),field3(uint64)' + \
            ',field4(float),field5(double),field6(complex)'
        writer = csv_writer('/tmp/file.csv', True, fields, 'uint8')

        # generate pdu
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('field0'), pmt.intern('field0'))
        metadata = pmt.dict_add(metadata, pmt.intern('field1'), pmt.from_bool(True))
        metadata = pmt.dict_add(metadata, pmt.intern('field2'), pmt.from_long(0))
        metadata = pmt.dict_add(metadata, pmt.intern('field3'), pmt.from_uint64(0))
        metadata = pmt.dict_add(metadata, pmt.intern('field4'), pmt.from_float(0.0))
        metadata = pmt.dict_add(metadata, pmt.intern('field5'), pmt.from_double(0.0))
        metadata = pmt.dict_add(metadata, pmt.intern('field6'), pmt.from_complex(1.0 + 1.0j))

        data = pmt.init_u8vector(5, [11, 12, 13, 14, 15])
        expected = pmt.cons(metadata, data)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, has_header=True))

    def test_006_precision(self):
        emitter = pdu_utils.message_emitter()
        writer = csv_writer('/tmp/file.csv', False, '', 'float', precision=4)

        # generate pdu
        metadata = pmt.PMT_NIL
        data = pmt.init_f32vector(2, [1.111111] * 2)
        sent = pmt.cons(metadata, data)
        expected = pmt.cons(pmt.PMT_NIL, pmt.init_f32vector(2, [1.1111] * 2))

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, data_type='float',
                                        has_header=False))

    def test_007_time(self):
        emitter = pdu_utils.message_emitter()
        writer = csv_writer('/tmp/file.csv', True, 'time(time)', 'uint8', precision=4)

        # generate time pair pdu
        time_pmt = pmt.cons(pmt.from_uint64(1), pmt.from_double(0.0))
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('time'), time_pmt)
        expected = pmt.cons(metadata, pmt.PMT_NIL)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, data_type='uint8',
                                        has_header=True))

    def test_008_time_tuple(self):
        emitter = pdu_utils.message_emitter()
        writer = csv_writer('/tmp/file.csv', True, 'time(time_tuple)', 'uint8', precision=4)

        # generate time pair pdu
        time_tuple = pmt.make_tuple(pmt.from_uint64(1), pmt.from_double(0.0))
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('time'), time_tuple)
        expected = pmt.cons(metadata, pmt.PMT_NIL)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, data_type='uint8',
                                        has_header=True))

    def test_009_float_precision(self):
        emitter = pdu_utils.message_emitter()
        fields = 'field0(long:03d),field1(uint64:03d),field2(float:.9f),field3(double:.9f),field4(complex:.9f)'
        writer = csv_writer('/tmp/file.csv', True, fields, 'uint8', precision=4)

        # generate time pair pdu
        metadata = pmt.dict_add(pmt.make_dict(), pmt.intern('field0'), pmt.from_long(1))
        metadata = pmt.dict_add(metadata, pmt.intern('field1'), pmt.from_uint64(1))
        metadata = pmt.dict_add(metadata, pmt.intern('field2'), pmt.from_float(1.234567890))
        metadata = pmt.dict_add(metadata, pmt.intern('field3'), pmt.from_double(1.234567890))
        metadata = pmt.dict_add(metadata, pmt.intern('field4'), pmt.from_complex(1.234567890 + 1.234567890j))
        expected = pmt.cons(metadata, pmt.PMT_NIL)

        # run
        tb = gr.top_block()
        tb.msg_connect((emitter, 'msg'), (writer, 'in'))
        tb.start()
        emitter.emit(expected)
        time.sleep(.5)
        tb.stop()
        tb.wait()

        # read in csv
        self.assertTrue(self.check_file('/tmp/file.csv', expected, data_type='uint8',
                                        has_header=True))


if __name__ == '__main__':
    gr_unittest.run(qa_csv_writer)
