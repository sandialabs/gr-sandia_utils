#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2019 gr-sandia_utils author.
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
import queue

class python_interface_sink(gr.sync_block):
    """
    Python Interface Sink

    Data bridge between the flowgraph and the controlling python script.

    This block uses an internal python Queue to hold incoming data. In order to
    empty the queue, call the get_data() function from the python flowgraph
    script. Note that if the internal queue fills up, it may halt the rest of
    the flowgraph.

    Inputs:
        in: any input stream

    Internal Functions:
        get_data(timeout):
            get the next data sample from the queue (optionally specify timeout
            in seconds, otherwise it will block)
        flush_data():
            empty the queue
    """
    def __init__(self, maxsize, vec_len):
        gr.sync_block.__init__(self,
            name="python_interface_sink",
            in_sig=[(numpy.float32, vec_len)],
            out_sig=None)
        self.maxsize = maxsize
        self.vec_len = vec_len
        self.dat_queue = queue.Queue(self.maxsize)

    def get_data(self, timeout = None):
        """ block until next data is available, then return it """
        return self.dat_queue.get(True, timeout)

    def flush_data(self):
        while True:
            try:
                self.dat_queue.get(False)
            except Queue.Empty:
                break

    def work(self, input_items, output_items):
        in0 = input_items[0]
        for i in range(len(in0)):
            try:
                self.dat_queue.put_nowait(in0[i])
            except Queue.Full:
                return i
        return len(in0)
