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


import numpy
from gnuradio import gr
import collections
import pmt
import tempfile
import itertools
import threading

class message_vector_csv_pdu(gr.sync_block):
    """
    Create CSV PDUs out of multiple input PDUs

    Takes in pdus and outputs pdus with csv data in the CDR portion
    representing a history of input pdus.

    @param key_field - key field for keeping separate histories. leave empty for no separation
    @param max_history - maximum number of pdus to keep, per unique key_field
    @param current_key_value - value of key_field to filter on
    @param metadata_fields - list of keys to extract from metadata
    @param skip_empty - only record PDUs with vector data
    """
    def __init__(self, key_field=pmt.intern("key_field"), max_history=1000, current_key_value=None, metadata_fields=[], skip_empty=True):
        gr.sync_block.__init__(self,
            name="message_vector_csv_pdu",
            in_sig=None,
            out_sig=None)

        # setup logger
        logger_name = 'gr_log.' + self.to_basic_block().alias()
        self.log = gr.logger(logger_name)

        self.max_history = max_history
        self.metadata_fields = metadata_fields
        self.skip_empty = skip_empty
        
        # key field can be empty/None, string, or pmt.intern
        if key_field == "" or key_field is None:
          self.key_field = None
        elif isinstance(key_field, str):
          self.key_field = pmt.intern(key_field)
        elif pmt.is_symbol(key_field):
          self.key_field = key_field
        else:
          raise(TypeError("key_field must be string or pmt.intern"))

        # storage of PDUs, key'ed by key_field
        self.pdu_data = {}
        self.pdu_data_mutex = threading.Lock()

        # set key filter to give messages from
        self.current_key_value = None

        # common use pmts
        self.pmt_pdu_in = pmt.intern('pdu_in')
        self.pmt_pdu_out = pmt.intern('pdu_out')

        # register message ports
        self.message_port_register_out(self.pmt_pdu_out)
        self.message_port_register_in(self.pmt_pdu_in)
        self.set_msg_handler(self.pmt_pdu_in, self.msg_handler)

    def set_current_key_value(self, current_key):
        self.current_key_value = current_key

    def set_skip_empty(self, skip_empty):
        self.skip_empty = skip_empty

    def set_max_history(self, max_history):
        with self.pdu_data_mutex:
          self.max_history = max_history
          for key,deq in self.pdu_data.items():
            deq.maxlen = self.max_history

    def set_metadata_fields(self, metadata_fields):
        with self.pdu_data_mutex:
          self.metadata_fields = metadata_fields

    def msg_handler(self, pdu):
        try:
          meta = pmt.car(pdu)
          data = pmt.cdr(pdu)
        except Exception as e:
          #print(e)
          pass

        # extract Key Field
        key = ""
        try:
          if self.key_field is not None and pmt.dict_has_key(meta, self.key_field):
            key = str(pmt.to_python(pmt.dict_ref(meta, self.key_field, pmt.PMT_NIL)))
        except TypeError as e:
          #print(e)
          return
        
        # key'ed off an id field, add before the empty check so an empty csv can return for no data
        if key not in self.pdu_data:
            self.pdu_data[key] = collections.deque(maxlen=self.max_history)

        # by default, don't store empty payloads
        if not pmt.length(data) and self.skip_empty:
          return

        # save pdu for later processing, no pmt-to-python data conversion at input time
        with self.pdu_data_mutex:
          self.pdu_data[key].append(pdu)


    def make_metadata_string(self, meta):
      csv_row = ""
      for key in self.metadata_fields:
        val = ""
        if pmt.dict_has_key(meta, pmt.intern(key)):
          val = pmt.to_python(pmt.dict_ref(meta, pmt.intern(key), pmt.PMT_NIL))
        csv_row += f"{key}:{val},"
      return csv_row

    def make_csv_string_2d(self, data):
        return "\n".join([",".join([f"{x}" for x in d]) for d in data]) + "\n"

    def request_csv_pdu(self, key=None, count=None):
        if count is None:
          count = self.max_history

        # current_key_value can specify the key/id if not explictly called here
        if key is None and self.current_key_value is not None:
          key = self.current_key_value
        elif key is None:
          key = ""

        if key not in self.pdu_data:
          self.log.error(f"key '{key}' not found in history, {self.pdu_data.keys()}")
          return
          
        with self.pdu_data_mutex:
          # no metadata fields case, convert data to csv-file binary data, most recent <count> only
          if len(self.metadata_fields) == 0:
            data = []
            for pdu in itertools.islice(reversed(self.pdu_data[key]),0,count):
              data.append(pmt.u8vector_elements(pmt.cdr(pdu)))
            csv_str = self.make_csv_string_2d(data)

          # When metadata fields are requested, iterate over each PDU
          else:
            csv_str = ""
            for pdu in itertools.islice(reversed(self.pdu_data[key]),0,count):
              meta = pmt.car(pdu)
              data = pmt.cdr(pdu)
              csv_str += self.make_metadata_string(meta)
              csv_str += self.make_csv_string_2d([pmt.u8vector_elements(data)])

        # publish result as PDU with the same "key_field" if given
        csv_bytes = csv_str.encode("utf-8")
        pmt_bytes = pmt.init_u8vector(len(csv_bytes), [int(x) for x in csv_bytes])
        meta = pmt.make_dict()
        if self.key_field is not None:
          meta = pmt.dict_add(meta, self.key_field, pmt.intern(key))
        self.message_port_pub(self.pmt_pdu_out, pmt.cons(meta,pmt_bytes))
        self.log.info(f"publishing a csv PDU for '{self.key_field}' : '{key}'")


