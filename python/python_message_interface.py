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
import queue

class python_message_interface(gr.basic_block):
    """
    Message bridge between the flowgraph and the controlling python script.
    """
    def __init__(self, queue_size):
        gr.basic_block.__init__(self,
            name="python_message_interface",
            in_sig=None,
            out_sig=None)
        self.message_port_register_in(pmt.intern('msg_in'))
        self.message_port_register_out(pmt.intern('msg_out'))
        self.set_msg_handler(pmt.intern('msg_in'), self.msg_handler)
        self.queue_size = queue_size
        self.msg_queue = queue.Queue(queue_size)

        def msg_handler(self, message):
            serialized_msg = pmt.serialize_str(message)
            while True:
                try:
                    self.msg_queue.put_nowait(serialized_msg)
                    break
                except queue.Full:
                    self.msg_queue.get(False)

        def get_message(self, timeout = None):
            """ block until next message is available, then return it """
            try:
                msg = self.msg_queue.get(True, timeout)
                return pmt.deserialize_str(msg)
            except queue.Empty:
                return None

        def send_message(self, message):
            """ send the given pmt message """
            self.message_port_pub(pmt.intern('msg_out'), message)

        def num_pending_messages(self):
            return self.msg_queue.qsize()

        def get_all_messages(self):
            messages = []
            while True:
                try:
                    next_msg = self.msg_queue.get_nowait()
                    messages.append(pmt.deserialize_str(next_msg))
                except queue.Empty:
                    break
            return messages

        def flush_messages(self):
            while True:
                try:
                    self.msg_queue.get(False)
                except queue.Empty:
                    break

