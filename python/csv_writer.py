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
import re
import copy
import math
from gnuradio import gr


class csv_writer(gr.sync_block):
    """
    Write data to CSV file.

    A header in the first row of the file is used to specify the format
    of the contents of the file.  Metadata fields are specified in the first
    columns of the file while the last columns of the file that do not have
    any header information associated with them are the data itself.

    If the file is selected to contain metadata, the fields to be output can
    be specified using a comma delimited string in the metadata format section
    or if left blank, all fields from the first received message will be
    output using an assumed string type.  Defining the metadata format is
    preferred not only to maintain data type consistency between CSV source
    and sink options, but also to be selective in the metadata that is output
    to the file.

    The metadata type is specified within parentheses after each metadata field.
    The string specification for the printing of the metadata can be specified
    using the 'field:spec' format.

    The input message must be a pair (PDU), with the metadata being the first
    element and a uniform vector the second element.

    The following metadata types and uniform vector types are supported:

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
      * uint64
      * int64
      * float
      * complex float
      * double
      * complex double

    Note: u64 and s64 vector elements are not fully supported in version
    3.7.13.3 and thus are not supported here yet.

    For floating point data types, the precision can also be specified.

    Given the following PDU with a metadata formatting of

      format: 'field0(string),field1(bool),field2(float)'
      data(uint8): [65, 66, 67, 68, 69, 70]


      (((field2 . 3.072e+07) (field1 . #t) (field0 . A)) . #[A B C D E F])

    Output file format (uint8 data type):

      field0(string),field1(bool),field2(float)
      A,True,30.72e6,65,66,67,68,69,70
    """

    def __init__(self, fname='', add_metadata=False, metadata_format='', data_type='uint8',
                 precision=0):
        gr.sync_block.__init__(self,
                               name="csv_writer",
                               in_sig=None,
                               out_sig=None)
        self.fname = fname
        self.add_metadata = add_metadata
        self.metadata_format = metadata_format
        self.data_type = data_type
        self.precision = precision
        self.fid = None

        # setup logger
        logger_name = 'gr_log.' + self.to_basic_block().alias()
        if logger_name in gr.logger_get_names():
            self.log = gr.logger(logger_name)
        else:
            self.log = gr.logger('log')

        # metadata field mappings
        self.metadata_mappings = {'string': lambda x: pmt.symbol_to_string(x),
                                  'bool': lambda x: pmt.to_bool(x),
                                  'long': lambda x: pmt.to_long(x),
                                  'uint64': lambda x: pmt.to_uint64(x),
                                  'float': lambda x: pmt.to_float(x),
                                  'double': lambda x: pmt.to_double(x),
                                  'complex': lambda x: pmt.to_complex(x),
                                  'time': lambda x: float(pmt.to_uint64(pmt.car(x)))
                                  + pmt.to_double(pmt.cdr(x)),
                                  'time_tuple': lambda x: float(pmt.to_uint64(pmt.tuple_ref(x, 0)))
                                  + pmt.to_double(pmt.tuple_ref(x, 1))
                                  }

        # data type parsers
        self.data_type_mappings = {'uint8': lambda x: pmt.u8vector_elements(x),
                                   'int8': lambda x: pmt.s8vector_elements(x),
                                   'uint16': lambda x: pmt.u16vector_elements(x),
                                   'int16': lambda x: pmt.s16vector_elements(x),
                                   'uint32': lambda x: pmt.u32vector_elements(x),
                                   'int32': lambda x: pmt.s32vector_elements(x),
                                   'float': lambda x: pmt.f32vector_elements(x),
                                   'complex float': lambda x: pmt.c32vector_elements(x),
                                   'double': lambda x: pmt.f64vector_elements(x),
                                   'complex double': lambda x: pmt.c64vector_elements(x)
                                   }

        # check data type
        if data_type not in self.data_type_mappings.keys():
            raise ValueError('Invalid data type')

        self.find_metadata = False
        self.header = []
        if self.add_metadata:
            if self.metadata_format == '':
                # set flag to load metadata on first message received
                self.find_metadata = True
            else:
                self.parse_header_format()

        # register message handler
        self.message_port_name = pmt.intern('in')
        self.message_port_register_in(self.message_port_name)
        self.set_msg_handler(self.message_port_name, self.message_handler)

    def message_handler(self, msg):
        if not pmt.is_dict(msg):
            return

        try:
            # this will fail if message is a PDU with non-PMT_NIL arguments
            n = pmt.length(pmt.dict_items(msg))

            # a PDU with one element equal to PMT_NIL still looks like a
            # dictionary...grrrrr!
            if (n == 1) and (pmt.equal(pmt.car(msg), pmt.PMT_NIL) or
                             pmt.equal(pmt.cdr(msg), pmt.PMT_NIL)):
                # treat as a pdu
                car = pmt.car(msg)
                cdr = pmt.cdr(msg)
            else:
                car = msg
                cdr = pmt.init_u8vector(0, [])
        except:
            try:
                # message is a pdu
                pmt.length(pmt.dict_items(pmt.car(msg)))
                car = pmt.car(msg)
                cdr = pmt.cdr(msg)
            except:
                return

        if self.find_metadata:
            keys = pmt.dict_keys(car)
            self.header = [(pmt.nth(i, keys), pmt.symbol_to_string) for i in range(pmt.length(keys))]

            header = ','.join([pmt.symbol_to_string(pmt.nth(i, keys)) for i in range(pmt.length(keys))])
            if self.fid:
                self.fid.write(header + '\n')

        # ensure we no longer search for metadata
        self.find_metadata = False

        if self.fid:
            # add metadata
            if self.add_metadata:
                self.print_metadata(car)

            # cdr must be a uniform vector type
            if not pmt.is_uniform_vector(cdr):
                self.fid.write('\n')
                return

            # add data
            values = self.data_type_mappings[self.data_type](cdr)
            if (self.precision > 0) and (self.data_type in ['float', 'double', 'complex float', 'complex double']):
                self.fid.write(','.join(['{:.{n}f}'.format(i, n=self.precision) for i in values]))
            else:
                self.fid.write(','.join([str(i) for i in values]))
                self.fid.write('\n')

    def start(self):
        try:
            self.fid = open(self.fname, 'w')
        except Exception as e:
            raise IOError("Unable to open output file {}".format(self.fname))

        # add header if specified
        if self.add_metadata and not self.find_metadata:
            self.fid.write(self.metadata_format)
            self.fid.write('\n')
        return True

    def stop(self):
        if self.fid:
            self.fid.close()

        return True

    def print_metadata(self, car):
        values = []
        for key, func in self.header:
            value = pmt.dict_ref(car, key, pmt.PMT_NIL)
            try:
                if pmt.equal(value, pmt.PMT_NIL):
                    values.append('')
                else:
                    values.append(func(value))
            except:
                values.append(pmt.write_string(value))

        # write to file
        self.fid.write(','.join(values))
        if len(values):
            # add trailing comma if metadata is present
            self.fid.write(',')

    def parse_header_format(self):
        # parse header
        try:
            header = self.metadata_format.split(',')
            keys = [x[0].rstrip().lstrip() for x in [re.findall(r'^[^\(]*', s) for s in header]]
            a = [re.findall(r'\((.*?)\)', s) for s in header]

            # default to string if unspecified - use specification string if given
            fields = [x[0].lstrip() if len(x) else 'string' for x in [re.findall(r'\((.*?)\)', s) for s in header]]

            # set header
            self.header = []
            stripped_fields = []
            for i in range(len(fields)):
                # load spec string if not specified
                values = fields[i].split(':')
                stripped_fields.append(values[0])
                if values[0] in self.metadata_mappings:
                    # basic print function
                    print_func = lambda x, values=values: str(self.metadata_mappings[values[0]](x))

                    if len(values) == 2:
                        spec = '{:' + values[1] + '}'
                        print_func = lambda x, spec=spec, values=values: spec.format(self.metadata_mappings[values[0]](x))

                    # add print function
                    self.header.append((pmt.intern(keys[i]), print_func))

            # update metadata format so it can be properly written to file
            self.metadata_format = ",".join([keys[i] + '(' + stripped_fields[i] + ')' for i in range(len(keys))])

        except Exception as e:
            raise ValueError("Unable to parse header format: {}".format(e))
