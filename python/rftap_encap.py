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


import array
import struct
import numpy
import pmt
from gnuradio import gr


class rftap_encap(gr.sync_block):
  """
  RFTap encapsulation - Add the rftap header to PDUs, see https://rftap.github.io/

  Create RFTap packet from PDU data and metadata
  See: https://rftap.github.io/

  Data Link Type (DLT) can be provided from input PDUs, as "dlt" key (integer).

  Alternatively, dissector name can be provided from input PDUs, as "dissector" key (string).

  Alternatively, DLT can be specified in this block parameters (integer).

  Alternatively, dissector name can be specified in this block parameters (string).

  Data Link Types (integer):
  http://www.tcpdump.org/linktypes.html

  Dissector (String):
  Name of wireshark dissector (or name of your custom lua/c dissector)
  """

  PMT_IN = pmt.intern('in')
  PMT_OUT = pmt.intern('out')

  def __init__(self, encapsulation_from, custom_dlt, custom_dissector_name):
      gr.basic_block.__init__(self,
          name="rftap_encap",
          in_sig=[],
          out_sig=[])

      # setup logger
      logger_name = 'gr_log.' + self.to_basic_block().alias()
      if logger_name in gr.logger_get_names():
        self.log = gr.logger(logger_name)
      else:
        self.log = gr.logger('log')

      self.encapsulation_from = encapsulation_from
      self.custom_dlt = custom_dlt
      self.custom_dissector_name = custom_dissector_name

      self.message_port_register_in(self.PMT_IN)
      self.set_msg_handler(self.PMT_IN, self.handle_msg)

      self.message_port_register_out(self.PMT_OUT)

  def handle_msg(self, pdu):
      if not pmt.is_pair(pdu):
          self.log.error("received invalid message type (not pair): {}".format(pdu))
          return

      d = pmt.to_python(pmt.car(pdu))  # metadata (dict)
      vec = pmt.to_python(pmt.cdr(pdu))  # data (u8 vector (numpy))

      if not d: d = {}  # fix for pmt.to_python(pmt.to_pmt({})) returning None...

      if not isinstance(d, dict):
          self.log.error("unexpected metadata type (not dict):{},{}".format(pdu, repr(d)))
          return

      if not isinstance(vec, numpy.ndarray) or not vec.dtype==numpy.dtype('uint8'):
          self.log.error("unexpected PDU data type (not ndarray uint8):{},{}".format(pdu, repr(v)))
          return

      vec = vec.tostring()  # aka tobytes() (numpy1.9+)

      flags = 0

      b = array.array('B');
      hdr = struct.pack('<4sHH', 'RFta'.encode(), 0, 0)  # len, flags written below
      b.frombytes(hdr)

      # this should be done in order of the bitfield:

      # dlt from PDU
      if self.encapsulation_from == 0:
          if 'dlt' not in d:
              self.log.error("dlt is expected in PDU, but it is missing")
          else:
              val = d.get('dlt')
              if not isinstance(val, (int)):
                  self.log.error("dlt in PDU is not an integer: {}".format(repr(val)))
              else:
                  b.frombytes(struct.pack('<I', val))
                  flags |= 1
      # custom dlt
      elif self.encapsulation_from == 2:
          val = self.custom_dlt
          if not isinstance(val, (int)):
              self.log.error("custom dlt is not an integer: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<I', val))
              flags |= 1

      # allow for 'freq' to be specified more explicitly as 'center_frequency'
      center_freq = None
      if 'freq' in d:
          center_freq = d.get('freq')
      elif 'center_frequency' in d:
          center_freq = d.get('center_frequency')
      if center_freq is not None:
          val = center_freq
          if not isinstance(val, (float,int)):
              self.log.error("freq is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<d', val))
              flags |= (1<<1)

      if 'nomfreq' in d:
          val = d.get('nomfreq')
          if not isinstance(val, (float,int)):
              self.log.error("nomfreq is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<d', val))
              flags |= (1<<2)

      if 'freqofs' in d:
          val = d.get('freqofs')
          if not isinstance(val, (float,int)):
              self.log.error("freqofs is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<d', val))
              flags |= (1<<3)

      if 'power' in d:
          val = d.get('power')
          if not isinstance(val, (float,int)):
              self.log.error("power is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<f', val))
              flags |= (1<<5)

      if 'noise' in d:
          val = d.get('noise')
          if not isinstance(val, (float,int)):
              self.log.error("noise is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<f', val))
              flags |= (1<<6)

      if 'snr' in d:
          val = d.get('snr')
          if not isinstance(val, (float,int)):
              self.log.error("snr is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<f', val))
              flags |= (1<<7)

      if 'qual' in d:
          val = d.get('qual')
          if not isinstance(val, (float,int)):
              self.log.error("qual is not a number: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<f', val))
              flags |= (1<<8)

      # time can be specified as 'start_time' or 'burst_time' in metadata
      pdu_time = None
      if 'start_time' in d:
        pdu_time = d.get('start_time')
      if 'burst_time' in d:
        pdu_time = d.get('burst_time')

      if pdu_time is not None:
          val = pdu_time
          if not isinstance(val, (float,int,tuple)):
              self.log.error("start_time is not a number: {}".format(repr(val)))
          else:
              if isinstance(val, (tuple)) and len(val) == 2:
                b.frombytes(struct.pack('<d', val[0]))
                b.frombytes(struct.pack('<d', val[1]))
                flags |= (1<<10)

              elif isinstance(val, (float)):
                b.frombytes(struct.pack('<d', int(val)))
                b.frombytes(struct.pack('<d', val-int(val)))
                flags |= (1<<10)

      # tagged parameters:

      # dissector name from PDU
      if self.encapsulation_from == 1:
          if 'dissector' not in d:
              self.log.error("dissector name is expected in PDU, but it is missing")
          else:
              val = d.get('dissector')
              if not isinstance(val, bytes) and not isinstance(val,str):
                  print(type(val))
                  self.log.error("dissector name in PDU is not a string: {}".format(repr(val)))
              else:
                  b.frombytes(struct.pack('<HBB', 16, len(val), 255))
                  b.frombytes(val.encode());
                  padlen = 3 - ((len(val)+3)&3)
                  padstr = '\0'*padlen
                  b.frombytes(padstr.encode())
      # custom dissector name
      elif self.encapsulation_from == 3:
          val = self.custom_dissector_name
          if not isinstance(val, bytes) and not isinstance(val,str):
              self.log.error("custom dissector name is not a string: {}".format(repr(val)))
          else:
              b.frombytes(struct.pack('<HBB', 16, len(val), 255))
              b.frombytes(val.encode("utf-8"));
              padlen = 3 - ((len(val)+3)&3)
              padstr = '\0'*padlen
              b.frombytes(padstr.encode())

      if len(b) % 4 != 0:
          self.log.error("wrong padding!!!")

      struct.pack_into('<H', b, 4, int(len(b)/4))
      struct.pack_into('<H', b, 6, flags)

      b.frombytes(vec)

      pmt_v = pmt.init_u8vector(len(b), b)
      outpdu = pmt.cons(pmt.car(pdu), pmt_v)
      self.message_port_pub(self.PMT_OUT, outpdu)
