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

        self.filename = filename

        # setup logger
        logger_name = 'gr_log.' + self.to_basic_block().alias()
        self.log = gr.logger(logger_name)

        # setup message handler
        self.message_port_register_in(pmt.intern("in"))
        self.set_msg_handler(pmt.intern("in"), self.handler)

    def start(self):
        try:
            self.file = open(self.filename, 'w+')
        except IOError as e:
            self.log.error("ERROR: could not open {}".format(filename))
            raise RuntimeError(f"IOError: Could not open file: {e}")

        return True

    def stop(self):
        if self.file:
            self.file.close()

        return True

    def handler(self, msg):
        try:
            # gnuradio-pmt 3.9 changed the output of the string generator
            # for pmts to now be utf-8 compatible so this should never
            # happen but just to be safe
            print(repr(msg), file=self.file)
        except UnicodeEncodeError as e:
            self.log.error('UnicodeEncodeError: {e}')
            print(repr(msg).encode('utf8', 'replace').decode(), file=self.file)
