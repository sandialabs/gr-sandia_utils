#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 gr-sandia_utils38 author.
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
from gnuradio import sandia_utils
from gnuradio import pdu_utils
import numpy as np
import time
import pmt

class qa_message_vector_csv_pdu(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.emitter = pdu_utils.message_emitter()
        self.csv = sandia_utils.message_vector_csv_pdu("key_field", 10)
        self.debug = blocks.message_debug()
        self.tb.msg_connect((self.emitter, 'msg'), (self.csv, 'pdu_in'))
        self.tb.msg_connect((self.csv, 'pdu_out'), (self.debug, 'store'))

    def tearDown(self):
        self.tb = None
        self.emitter = None
        self.csv = None
        self.debug = None

    def test_001_single_key(self):
        # generate pdu
        v = np.array([48, 49, 50], dtype='uint8')
        d = pmt.dict_add(pmt.make_dict(), pmt.intern("key_field"), pmt.intern("key1"))
        p = pmt.cons(d, pmt.init_u8vector(len(v), v.tolist()))

        # run flowgraph
        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(p)
        time.sleep(0.002)
        self.emitter.emit(p)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU result
        #00000000: 3438 2c34 392c 3530 0a34 382c 3439 2c35  48,49,50.48,49,5
        #00000010: 300a                                     0.
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("34382c34392c35300a34382c34392c35300a")
        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))

    def test_002_two_keys(self):
        # generate pdu
        v1 = np.array([48, 49, 50], dtype='uint8')
        d1 = pmt.make_dict()
        d1 = pmt.dict_add(d1, pmt.intern("key_field"), pmt.intern("key1"))

        v2 = np.array([1, 2, 3], dtype='uint8')
        d2 = pmt.make_dict()
        d2 = pmt.dict_add(d2, pmt.intern("key_field"), pmt.intern("key2"))

        p1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        p2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))

        # run flowgraph
        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(p2)
        self.emitter.emit(p1)
        time.sleep(0.002)
        self.emitter.emit(p1)
        self.emitter.emit(p2)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU result
        #00000000: 3438 2c34 392c 3530 0a34 382c 3439 2c35  48,49,50.48,49,5
        #00000010: 300a                                     0.
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("34382c34392c35300a34382c34392c35300a")
        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))

    def test_003_order_count(self):
        # generate pdu
        v1 = np.array([48, 49, 50], dtype='uint8')
        d1 = pmt.dict_add(pmt.make_dict(), pmt.intern("key_field"), pmt.intern("key1"))
        v2 = np.array([51, 52, 53], dtype='uint8')
        d2 = pmt.dict_add(pmt.make_dict(), pmt.intern("key_field"), pmt.intern("key1"))
        p1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        p2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))

        # run flowgraph
        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(p1)
        time.sleep(0.002)
        self.emitter.emit(p1)
        time.sleep(0.002)
        self.emitter.emit(p2)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1", count=2)
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU result
        #00000000: 3531 2c35 322c 3533 0a34 382c 3439 2c35  51,52,53.48,49,5
        #00000010: 300a                                     0.
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("35312c35322c35330a34382c34392c35300a")
        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))

    def test_004_skip_empty(self):
        # generate pdu
        v1 = np.array([48, 49, 50], dtype='uint8')
        d1 = pmt.dict_add(pmt.make_dict(), pmt.intern("key_field"), pmt.intern("key1"))
        v2 = np.array([], dtype='uint8')
        d2 = pmt.dict_add(pmt.make_dict(), pmt.intern("key_field"), pmt.intern("key1"))
        p1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        p2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))

        # run flowgraph
        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(p1)
        time.sleep(0.002)
        self.emitter.emit(p2)
        time.sleep(0.002)
        self.emitter.emit(p1)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU result
        #00000000: 3438 2c34 392c 3530 0a34 382c 3439 2c35  48,49,50.48,49,5
        #00000010: 300a                                     0.
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("34382c34392c35300a34382c34392c35300a")
        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))

    def test_005_empty_file(self):
        # generate pdu
        v1 = np.array([], dtype='uint8')
        d1 = pmt.dict_add(pmt.make_dict(), pmt.intern("key_field"), pmt.intern("key1"))
        p1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))

        # run flowgraph
        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(p1)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU result
        # empty file
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("0a") # end with newline always
        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))

    def test_006_metadata_fields(self):
        self.csv.set_max_history(2)
        self.csv.set_metadata_fields(["App","Bass","Dog"])
        # generate pdu
        v1 = np.array([48,49,50], dtype='uint8')
        d1 = pmt.make_dict()
        d1 = pmt.dict_add(d1, pmt.intern("key_field"), pmt.intern("key1"))
        d1 = pmt.dict_add(d1, pmt.intern("App"), pmt.from_long(1))
        d1 = pmt.dict_add(d1, pmt.intern("Bass"), pmt.from_long(2))
        d1 = pmt.dict_add(d1, pmt.intern("Dog"), pmt.from_long(3))

        v2 = np.array([51,52,53], dtype='uint8')
        d2 = pmt.make_dict()
        d2 = pmt.dict_add(d2, pmt.intern("key_field"), pmt.intern("key1"))
        d2 = pmt.dict_add(d2, pmt.intern("App"), pmt.from_long(4))
        d2 = pmt.dict_add(d2, pmt.intern("Bass"), pmt.from_long(5))
        d2 = pmt.dict_add(d2, pmt.intern("Dog"), pmt.from_long(6))

        pdu1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        pdu2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))

        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(pdu1)
        self.emitter.emit(pdu1)
        self.emitter.emit(pdu1)
        time.sleep(0.002)
        self.emitter.emit(pdu2)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU results
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        # App:4,Bass:5,Dog:6,51,52,53
        # App:1,Bass:2,Dog:3,48,49,50
        expected = bytes.fromhex("4170703a342c426173733a352c446f673a362c35312c35322c35330a4170703a312c426173733a322c446f673a332c34382c34392c35300a")

        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))

    def test_007_missing_metadata_fields(self):
        self.csv.set_max_history(10)
        self.csv.set_metadata_fields(["App","Bass","Dog"])
        # generate pdu
        v1 = np.array([48,49,50], dtype='uint8')
        v2 = np.array([51,52,53], dtype='uint8')
        
        d1 = pmt.make_dict()
        d1 = pmt.dict_add(d1, pmt.intern("key_field"), pmt.intern("key88"))
        d1 = pmt.dict_add(d1, pmt.intern("App"), pmt.from_long(1))
        d1 = pmt.dict_add(d1, pmt.intern("Bass"), pmt.from_long(2))
        #d = pmt.dict_add(d, pmt.intern("Dog"), pmt.PMT_NIL)

        d2 = pmt.make_dict()
        d2 = pmt.dict_add(d2, pmt.intern("key_field"), pmt.intern("key88"))
        d2 = pmt.dict_add(d2, pmt.intern("App"), pmt.from_long(4))
        d2 = pmt.dict_add(d2, pmt.intern("Bass"), pmt.from_long(5))
        d2 = pmt.dict_add(d2, pmt.intern("Dog"), pmt.from_long(6))

        pdu1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        pdu2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))

        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(pdu1)
        time.sleep(0.002)
        self.emitter.emit(pdu2)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key88")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU results
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        #App:4,Bass:5,Dog:6,51,52,53
        #App:1,Bass:2,Dog:,48,49,50
        expected = bytes.fromhex("4170703a342c426173733a352c446f673a362c35312c35322c35330a4170703a312c426173733a322c446f673a2c34382c34392c35300a")

        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))



    def test_008_metadata_fields_Max_History_check(self):
        self.csv.set_max_history(2)
        self.csv.set_metadata_fields(["App","Bass","Dog"])
        # generate pdu
        v1 = np.array([48,49,50], dtype='uint8')
        d1 = pmt.make_dict()
        d1 = pmt.dict_add(d1, pmt.intern("key_field"), pmt.intern("key1"))
        d1 = pmt.dict_add(d1, pmt.intern("App"), pmt.from_long(1))
        d1 = pmt.dict_add(d1, pmt.intern("Bass"), pmt.from_long(2))
        d1 = pmt.dict_add(d1, pmt.intern("Dog"), pmt.PMT_NIL)

        v2 = np.array([51,52,53], dtype='uint8')
        d2 = pmt.make_dict()
        d2 = pmt.dict_add(d2, pmt.intern("key_field"), pmt.intern("key1"))
        d2 = pmt.dict_add(d2, pmt.intern("App"), pmt.from_long(4))
        d2 = pmt.dict_add(d2, pmt.intern("Bass"), pmt.from_long(5))
        d2 = pmt.dict_add(d2, pmt.intern("Dog"), pmt.from_long(6))
        
        v3 = np.array([54,55,56], dtype='uint8')
        d3 = pmt.make_dict()
        d3 = pmt.dict_add(d3, pmt.intern("key_field"), pmt.intern("key1"))
        d3 = pmt.dict_add(d3, pmt.intern("App"),  pmt.from_long(7))
        d3 = pmt.dict_add(d3, pmt.intern("Bass"), pmt.from_long(8))
        d3 = pmt.dict_add(d3, pmt.intern("Dog"),  pmt.from_long(9))

        pdu1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        pdu2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))
        pdu3 = pmt.cons(d3, pmt.init_u8vector(len(v3), v3.tolist()))

        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(pdu1)
        time.sleep(0.002)
        self.emitter.emit(pdu2)
        time.sleep(0.002)
        self.emitter.emit(pdu3)
        time.sleep(0.002)
        self.emitter.emit(pdu3)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU results
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("4170703a312c426173733a322c446f673a2c34382c34392c35300a4170703a342c426173733a352c446f673a362c35312c35322c35330a")


    def test_009_metadata_fields_select_and_Max_History(self):
        self.csv.set_metadata_fields(["App","Bass","Dog"])
        # generate pdu
        v1 = np.array([48,49,50], dtype='uint8')
        d1 = pmt.make_dict()
        d1 = pmt.dict_add(d1, pmt.intern("key_field"), pmt.intern("key2"))
        d1 = pmt.dict_add(d1, pmt.intern("App"), pmt.from_long(1))
        d1 = pmt.dict_add(d1, pmt.intern("Bass"), pmt.from_long(2))
        d1 = pmt.dict_add(d1, pmt.intern("Dog"), pmt.PMT_NIL)

        v2 = np.array([51,52,53], dtype='uint8')
        d2 = pmt.make_dict()
        d2 = pmt.dict_add(d2, pmt.intern("key_field"), pmt.intern("key1"))
        d2 = pmt.dict_add(d2, pmt.intern("App"), pmt.from_long(4))
        d2 = pmt.dict_add(d2, pmt.intern("Bass"), pmt.from_long(5))
        d2 = pmt.dict_add(d2, pmt.intern("Dog"), pmt.from_long(6))

        v3 = np.array([54,55,56], dtype='uint8')
        d3 = pmt.make_dict()
        d3 = pmt.dict_add(d3, pmt.intern("key_field"), pmt.intern("key3"))
        d3 = pmt.dict_add(d3, pmt.intern("App"),  pmt.from_long(7))
        d3 = pmt.dict_add(d3, pmt.intern("Bass"), pmt.from_long(8))
        d3 = pmt.dict_add(d3, pmt.intern("Dog"),  pmt.from_long(9))

        pdu1 = pmt.cons(d1, pmt.init_u8vector(len(v1), v1.tolist()))
        pdu2 = pmt.cons(d2, pmt.init_u8vector(len(v2), v2.tolist()))
        pdu3 = pmt.cons(d3, pmt.init_u8vector(len(v3), v3.tolist()))

        self.tb.start()
        time.sleep(0.002)
        self.emitter.emit(pdu1)
        time.sleep(0.002)
        self.emitter.emit(pdu2)
        time.sleep(0.002)
        self.emitter.emit(pdu3)
        time.sleep(0.002)
        self.emitter.emit(pdu3)
        time.sleep(0.002)
        self.emitter.emit(pdu3)
        time.sleep(0.002)
        self.csv.request_csv_pdu("key1")
        time.sleep(0.05)
        self.tb.stop()
        self.tb.wait()

        # validate PDU results
        self.assertEqual(1, self.debug.num_messages())
        rcv = pmt.u8vector_elements(pmt.cdr(self.debug.get_message(0)))
        expected = bytes.fromhex("4170703a342c426173733a352c446f673a362c35312c35322c35330a")

        self.assertEqual(len(rcv), len(expected))
        self.assertTrue(all([a == b for a,b in zip(rcv, expected)]))


if __name__ == '__main__':
    gr_unittest.run(qa_message_vector_csv_pdu)
