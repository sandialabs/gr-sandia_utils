#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


import numpy
from gnuradio import gr

import numpy
import pmt
from gnuradio import gr

class message_file_debug(gr.basic_block):
    """
    Message File Debug

    Human-readable printing of messages to specified output file.  Messages
    are printed using the PMT string representation.
    """
    def __init__(self, filename):
        gr.basic_block.__init__(self,
            name="message_file_debug",
            in_sig=None,
            out_sig=None)
        self.message_port_register_in(pmt.intern("in"))
        self.set_msg_handler(pmt.intern("in"), self.handler)
        try:
            self.file = open(filename, 'w+')
        except IOError:
            print("ERROR: could not open {}".format(filename))
            quit()

    def __del__(self):
      self.file.close()

    def handler(self, msg):
      print(repr(msg),file=self.file)
