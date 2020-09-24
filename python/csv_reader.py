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
import os
import re
import pmt
import threading
import math


class csv_reader(gr.sync_block):
    """
    Read data from CSV file.

    A header in the first row of the file is used to specify the format
    of the contents of the file.  Metadata fields are specified in the first
    columns of the file while the last columns of the file that do not have
    any header information associated with them will be used to populate the
    second element in the PDU as a uniform vector based on the selected data
    type.

    If the file is said not to contain a header, all columns will attempt to
    be read into a uniform vector of the specified type.

    Note: 'Empty' header fields will not generate metadata fields

    Note: u64 and s64 vector elements are not fully supported in version
    3.7.13.3 and thus are not supported here yet.

    Supported metadata types:
      * string
      * bool (True/False)
      * long
      * uint64
      * float
      * double
      * complex
      * time (car = uint64, cdr = double)
      * time_tuple(0 - uint64, 1 - double)

    Supported data types:
      * uint8
      * int8
      * uint16
      * int16
      * uint32
      * int32
      * float
      * complex float
      * double
      * complex double

    Sample file format (uint8 data type):

    field0(string), , field1(bool), field2(float)
    A,B,True,30.72e6,65,66,67,68,69,70

    Output:
      (((field2 . 3.072e+07) (field1 . #t) (field0 . A)) . #[A B C D E F])
    """

    def __init__(self, fname='', has_header=True, data_type='uint8',
                 period=1000, start_delay=0, repeat=True):
        gr.sync_block.__init__(self, "CSV Reader",
                               in_sig=None, out_sig=None)
        self.file = fname
        self.has_header = has_header
        self.data_type = data_type
        self.period = period
        self.repeat = repeat
        self.start_delay = start_delay

        # setup logger
        logger_name = 'gr_log.' + self.to_basic_block().alias()
        if logger_name in gr.logger_get_names():
            self.log = gr.logger(logger_name)
        else:
            self.log = gr.logger('log')

        # metadata field mappings
        self.metadata_mappings = {'string': lambda x: pmt.intern(x),
                                  'bool': lambda x: pmt.from_bool(bool(x)),
                                  'long': lambda x: pmt.from_long(int(x)),
                                  'uint64': lambda x: pmt.from_uint64(int(x)),
                                  'float': lambda x: pmt.from_float(float(x)),
                                  'double': lambda x: pmt.from_double(float(x)),
                                  'complex': lambda x: pmt.from_complex(complex(x)),
                                  'time': lambda x: pmt.cons(pmt.from_uint64(int(math.modf(float(x))[1])),
                                                             pmt.from_double(math.modf(float(x))[0])),
                                  'time_tuple': lambda x: pmt.make_tuple(pmt.from_uint64(int(math.modf(float(x))[1])),
                                                                         pmt.from_double(math.modf(float(x))[0]))
                                  }

        self.data_type_mappings = {'uint8': lambda x: pmt.init_u8vector(len(x), [int(y) for y in x]),
                                   'int8': lambda x: pmt.init_s8vector(len(x), [int(y) for y in x]),
                                   'uint16': lambda x: pmt.init_u16vector(len(x), [int(y) for y in x]),
                                   'int16': lambda x: pmt.init_s16vector(len(x), [int(y) for y in x]),
                                   'uint32': lambda x: pmt.init_u32vector(len(x), [int(y) for y in x]),
                                   'int32': lambda x: pmt.init_s32vector(len(x), [int(y) for y in x]),
                                   'float': lambda x: pmt.init_f32vector(len(x), [float(y) for y in x]),
                                   'complex float': lambda x: pmt.init_c32vector(len(x), [complex(y) for y in x]),
                                   'double': lambda x: pmt.init_f64vector(len(x), [float(y) for y in x]),
                                   'complex double': lambda x: pmt.init_c64vector(len(x), [complex(y) for y in x])
                                   }

        # ensure valid data type
        if self.data_type not in self.data_type_mappings.keys():
            raise ValueError('Invalid data type {}'.format(data_type))

        # set file name
        self.set_fname(self.file)

        self.message_port_name = pmt.intern('out')
        self.message_port_register_out(self.message_port_name)

        # flag for when to stop
        self.stop_signal_called = False

        # no timer yet
        self.timer = None

        # file lock
        self.lock = threading.Lock()

    def parse_line(self, line):
        # parse values
        values = [x.lstrip() for x in line.rstrip().rstrip(',').split(',')]

        # add metadata
        dict = pmt.make_dict()
        ind = 0
        for t in self.header:
            if t != ():
                try:
                    dict = pmt.dict_add(dict, t[0], t[1](values[ind]))
                except Exception as e:
                    self.log.error("Unable to parse header: {}".format(e))

            # move to next element
            ind += 1

        # load data
        data = values[ind:]
        if len(data):
            try:
                data = self.data_type_mappings[self.data_type](values[ind:])
            except Exception as e:
                self.log.error("Unable to parse data: {}".format(e))
                data = pmt.PMT_NIL
        else:
            data = pmt.PMT_NIL

        return pmt.cons(dict, data)

    def get_line(self):
        with self.lock:
            # continue reading
            line = self.fid.readline()
            start_timer = True
            if line:
                pdu = self.parse_line(line)
                if pdu:
                    self.message_port_pub(self.message_port_name, pdu)
            else:
                if self.repeat:
                    # move back to beginning of file
                    self.fid.seek(0)

                    # reload header
                    self.load_header()
                else:
                    # no need to start timer
                    start_timer = False

            if start_timer:
                self.timer = threading.Timer(self.period / 1000., self.get_line)
                self.timer.start()

    def start(self):
        # start thread to produce data
        if self.start_delay:
            self.timer = threading.Timer(self.start_delay / 1000., self.get_line)
        else:
            self.timer = threading.Timer(self.period / 1000., self.get_line)
        self.timer.start()
        return True

    def stop(self):
        if self.timer:
            self.timer.cancel()

        with self.lock:
            self.fid.close()

        return True

    def set_start_delay(self, delay):
        self.start_delay = delay

    def get_start_delay(self):
        return self.start_delay

    def set_period(self, period):
        self.period = period

    def get_period(self):
        return self.period

    def set_repeat(self, repeat):
        self.repeat = repeat

    def get_repeat(self):
        return self.repeat

    def set_fname(self, fname):
        # ensure file exists
        self.file = fname
        if not os.path.isfile(self.file):
            raise IOError("Invalid input file {}".format(self.file))

        self.fid = open(self.file, 'r')
        if not self.fid:
            raise IOError("Unable to open input file {}".format(self.file))

        # load header
        self.load_header()

    def get_fname(self):
        return self.file

    def load_header(self):
        # parse header
        self.header = []
        if self.has_header:
            try:
                header = self.fid.readline().rstrip(',').split(',')
                keys = [x[0].rstrip().lstrip() for x in [re.findall(r'^[^\(]*', s) for s in header]]
                a = [re.findall(r'\((.*?)\)', s) for s in header]

                # default to string if unspecified
                values = [x[0].lstrip() if len(x) else 'string' for x in [re.findall(r'\((.*?)\)', s) for s in header]]

                # set header
                self.header = [(pmt.intern(keys[i]), self.metadata_mappings[values[i]])
                               if keys[i] != '' else () for i in range(len(header))]

            except Exception as e:
                raise ValueError("Unable to parse header: {}".format(e))
