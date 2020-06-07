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
import sandia_utils_swig as sandia_utils
import pdu_utils
import pmt
import time

class qa_invert_tune(gr_unittest.TestCase):

  def setUp (self):
      self.tb = gr.top_block ()
      self.emitter = pdu_utils.message_emitter()
      self.i_t = sandia_utils.invert_tune()
      self.debug = blocks.message_debug()
      self.tb.msg_connect((self.emitter, 'msg'), (self.i_t, 'tune'))
      self.tb.msg_connect((self.i_t, 'tune'), (self.debug, 'store'))

  def tearDown (self):
      self.tb = None

  def test_001_t (self):
      tune1 = pmt.dict_add(pmt.make_dict(), pmt.intern('dsp_freq'), pmt.from_double(1.23456789))
      expected_tune = pmt.dict_add(pmt.make_dict(), pmt.intern('dsp_freq'), pmt.from_double(-1.23456789))

      self.tb.start()
      time.sleep(.001)
      self.emitter.emit(tune1)
      time.sleep(.01)
      self.tb.stop()
      #self.tb.wait() # don't wait...may not return in time

      self.assertTrue(pmt.equal(self.debug.get_message(0), expected_tune))

  def test_002_t (self):
      tune1 = pmt.dict_add(pmt.make_dict(), pmt.intern('dsp_freq'), pmt.from_double(-999e9))
      tune1 = pmt.dict_add(tune1, pmt.intern('other_freq'), pmt.from_double(999e9))
      expected_tune = pmt.dict_add(pmt.make_dict(), pmt.intern('other_freq'), pmt.from_double(999e9))
      expected_tune = pmt.dict_add(expected_tune, pmt.intern('dsp_freq'), pmt.from_double(999e9))

      self.tb.start()
      time.sleep(.001)
      self.emitter.emit(pmt.intern("BAD PDU"))
      time.sleep(.001)
      self.emitter.emit(tune1)
      time.sleep(.01)
      self.tb.stop()
      #self.tb.wait() # don't wait...may not return in time

      self.assertTrue(pmt.equal(self.debug.get_message(0), expected_tune))


if __name__ == '__main__':
    gr_unittest.run(qa_invert_tune)
