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

import os
import shutil
import numpy as np
import threading
import collections
import time
import datetime
import copy
import glob

from gnuradio import gr
import pmt

class file_archiver(gr.basic_block):
  """
  File archiver.

  Archive generated file data.  This block is intended to receive messages
  from the file sink block indicating that a new file has been written.  To
  allow for the file sink block to not be concerned with the generation of a
  unique file name, the archiver can be used in conjunction with the block to
  save files to an alternate location using a uniquely generated name.

  If the filename format is unspecified, files will be directly copied to the
  output directory.  Otherwise, metadata associated with the filename can be
  used to generate a unique filename on output.

  File names can be specified using all conversion specifications
  supported by strftime.  Additionally, the following converation
  specifications can be used:

    %fcM      Collection center frequency in MHz
    %fck      Collection center frequency in kHz
    %fcc      Collection center frequency in Hz
    %fsM      Collection sample rate in MHz
    %fsk      Collection sample rate in kHz
    %fsc      Collection sample rate in Hz
  """
  def __init__(self, output_path="/tmp", fname_format = "", archive = False):
    gr.basic_block.__init__(self,
        name="file_archiver",
        in_sig=None,
        out_sig=None)
    self.message_port_register_in(pmt.intern("pdu"))
    self.set_msg_handler(pmt.intern("pdu"), self.handler)

    # setup logger
    logger_name = 'gr_log.' + self.to_basic_block().alias()
    if logger_name in gr.logger_get_names():
      self.log = gr.logger(logger_name)
    else:
      self.log = gr.logger('log')

    # protection mutex
    self.mutex = threading.Lock()

    # file pdu dictionary elements
    self.filename_tag = pmt.intern('fname')
    self.freq_tag = pmt.intern("rx_freq")
    self.rate_tag = pmt.intern("rx_rate")
    self.time_tag = pmt.intern("rx_time")

    # frequency specifiers
    self.freq_specs = [('%fcM',1e6),('%fck',1e3),('%fcc',1.)]

    # rate specifiers
    self.rate_specs = [('%fsM',1e6),('%fsk',1e3),('%fsc',1.)]

    # where to copy files
    self.set_output_path(output_path)

    # filename format
    self.set_fname_format(fname_format)

    # set archive
    self.set_archive(archive)

  def set_output_path(self,output_path):
    with self.mutex:
      if os.path.isdir(output_path):
        self.output_path = output_path
      else:
        try:
          self.log.debug("Generating output path {}".format(output_path))
          os.mkdir(output_path)
          self.output_path = output_path
        except OSError:
          raise ValueError('unable to create output path {}'.format(output_path))

  def get_output_path(self):
    return self.output_path

  def set_fname_format(self,fname_format):
    with self.mutex:
      self.fname_format = fname_format

  def get_fname_format(self):
    return self.fname_format

  def set_archive(self,archive):
    with self.mutex:
      self.archive = archive

  def get_archive(self):
    return self.archive

  def copy_file(self,source,destination):
    self.log.debug("Copying file {} to {}".format(source,destination))
    # check if file already exists
    if os.path.exists(destination):
      # find last file created, by finding last extension
      files = glob.glob(destination + '.*')
      destination = destination + ('.%d' % len(files))
      self.log.warn("File already exists...using {} instead".format(destination))

    # copy file
    shutil.copyfile(source,destination)

  def handler(self, msg):
    if not self.archive:
      # no need to waste any time
      return

    if not pmt.is_dict(msg):
      return

    try:
      # this will fail if message is a PDU with non-PMT_NIL arguments
      n = pmt.length(pmt.dict_items(msg))

      # a PDU with one element equal to PMT_NIL still looks like a
      # dictionary...grrrrr!
      if (n == 1) and (pmt.equal(pmt.car(msg),pmt.PMT_NIL) or \
                       pmt.equal(pmt.cdr(msg),pmt.PMT_NIL)):
        # treat as a pdu
        meta = pmt.car(msg)
      else:
        # it's a dictionary
        meta = msg
    except:
      try:
        # message is a pdu
        pmt.length(pmt.dict_items(pmt.car(msg)))
        meta = pmt.car(msg)
      except:
        return

    # extract file components
    try:
      fname = pmt.dict_ref(meta,self.filename_tag,pmt.PMT_NIL)
      file_time = pmt.dict_ref(meta,self.time_tag,pmt.PMT_NIL)
      freq = pmt.dict_ref(meta,self.freq_tag,pmt.PMT_NIL)
      rate = pmt.dict_ref(meta,self.rate_tag,pmt.PMT_NIL)

      if pmt.equal(fname,pmt.PMT_NIL):
        self.log.warn("No file specified")
        return

      f = pmt.symbol_to_string(fname)
      if self.fname_format == "": # copy immediately
        self.copy_file(f,os.path.join(self.output_path,os.path.basename(f)))
      else:
        base_fname = copy.deepcopy(self.fname_format)

        # add frequency information to file name
        if not pmt.equal(freq,pmt.PMT_NIL):
          freq = pmt.to_double(freq)
          for freq_spec in self.freq_specs:
            base_fname = base_fname.replace(freq_spec[0],'%0.0f' % int(freq / freq_spec[1]))


        if not pmt.equal(rate,pmt.PMT_NIL):
          rate = pmt.to_double(rate)
          for rate_spec in self.rate_specs:
            base_fname = base_fname.replace(rate_spec[0],'%0.0f' % int(rate / rate_spec[1]))

        # time update
        if not pmt.equal(file_time,pmt.PMT_NIL):
          t = pmt.to_uint64(pmt.tuple_ref(file_time,0)) + \
            pmt.to_double(pmt.tuple_ref(file_time,1))
          base_fname = datetime.datetime.utcfromtimestamp(t).strftime(base_fname)

        # archive file
        self.copy_file(f, os.path.join(self.output_path,base_fname))
    except Exception as e:
      self.log.error("Unable to process message:{}".format(e))
