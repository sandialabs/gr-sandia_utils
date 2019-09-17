#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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
import pmt
from gnuradio import gr

class message_file_debug(gr.basic_block):
    """
    Write messages to specified file
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
            print "ERROR: could not open ", filename
            quit()

    def __del__(self):
        self.file.close()

    def handler(self, msg):
        print >> self.file, msg

