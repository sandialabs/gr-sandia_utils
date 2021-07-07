#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


import os
import shutil
import numpy as np
import threading
import collections
import time
import copy

from gnuradio import gr
import pmt

class file_monitor(gr.basic_block):
  """
  File Monitor

  This block is meant to monitor files generated using the file sink block which
  emits file names on message generation.  The block will allow for the retention
  of a specified number of files before removing them from the system.  At any
  point, the block can be signaled to copy the current monitored data to another
  location rather than removing from their current location.
  """
  def __init__(self, nfiles_max=10, output_path="/tmp", copy = False):
    gr.basic_block.__init__(self,
        name="file_monitor",
        in_sig=None,
        out_sig=None)
    self.message_port_register_in(pmt.intern("pdu"))
    self.set_msg_handler(pmt.intern("pdu"), self.handler)

    # protection mutex
    self.mutex = threading.Lock()

    # setup logger
    logger_name = 'gr_log.' + self.to_basic_block().alias()
    if logger_name in gr.logger_get_logger_names():
      self.log = gr.logger(logger_name)
    else:
      self.log = gr.logger('log')

    # file name tags
    self.filename_tag = pmt.intern('fname')

    # where to copy files
    self.set_output_path(output_path)

    # copy files flag
    self.copy = copy

    # file queue
    self.file_queue = collections.deque([])
    self.copy_queue = collections.deque([])
    self.nfiles_max = nfiles_max

    # thread to flush data from monitor, either moving to the specified location
    # or removing from the system
    self.file_flusher = threading.Thread(target=self.flush)
    self.stop_signal_called = False

  def start(self):
    self.file_flusher.start()

    return True

  def stop(self):
    """ Ensure thread is shut down properly """
    self.stop_signal_called = True

    # wait for thread to tx_complete
    self.file_flusher.join()

    # clear left over files - not guaranteed to remove all files, only
    # those that have been published and received
    while len(self.file_queue):
      fname = self.file_queue.popleft()
      self.delete(fname)

    return True

  def set_copy(self,copy_flag):
    if (not self.copy and copy_flag):
      # starting copying
      self.copy = copy_flag

      # copy files from file queue
      with self.mutex:
        self.copy_queue = copy.deepcopy(self.file_queue)
        self.log.debug('number of items to copy: ',len(self.copy_queue))

    elif (self.copy and not copy_flag):
      # stopping copying
      self.copy = copy_flag

      # clear queue
      with self.mutex:
        self.copy_queue.clear()

  def get_copy(self):
    return self.copy

  def set_nfiles(self,nfiles):
    if nfiles > 0:
      self.nfiles_max = nfiles

      # remove if necessary
      with self.mutex:
        while len(self.file_queue) > self.nfiles_max:
          oldest = self.file_queue.popleft()
          self.delete(oldest)

  def get_nfiles(self):
    return self.nfiles_max

  def set_output_path(self,path):
    with self.mutex:
      if os.path.isdir(path):
        self.output_path = path
      else:
        # try to create path
        try:
          self.log.debug("generating output path {}".format(path))
          os.mkdir(path)
          self.output_path = path
        except OSError:
          raise ValueError('unable to create output path {}'.format(path))

  def get_output_path(self):
    return self.output_path

  def flush(self):
    while not self.stop_signal_called:
      if not self.copy:
        time.sleep(.5)
        continue

      if len(self.copy_queue):
        with self.mutex:
          fname = self.copy_queue.popleft()

        # get filename
        file = os.path.basename(fname)
        out_file = os.path.join(self.output_path,file)

        # copy
        try:
          self.log.debug("copying file {} to {}".format(fname,out_file))
          shutil.copy(fname,out_file)
        except Exception as e:
          self.log.error("unable to copy file: {}".format(e))

      # TODO: Make this dynamic prevent thread lock
      if not len(self.copy_queue):
        time.sleep(.5)

  def delete(self,fname):
    try:
      self.log.debug("removing file {}".format(fname))
      os.remove(fname)
    except Exception as e:
      self.log.error("unable to delete file: {}".format(e))

  def handler(self, msg):
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
        dict = pmt.car(msg)
      else:
        dict = msg
    except:
      try:
        # message is a pdu
        pmt.length(pmt.dict_items(pmt.car(msg)))
        dict = pmt.car(msg)
      except:
        return

    # attempt to extract file name
    fpmt = pmt.dict_ref(dict,self.filename_tag,pmt.PMT_NIL)
    if pmt.equal(fpmt, pmt.PMT_NIL):
      return
    fname = pmt.symbol_to_string(fpmt)
    self.log.debug("received file {}".format(fname))

    with self.mutex:
      self.file_queue.append(fname)

      if self.copy:
        self.copy_queue.append(fname)

      if len(self.file_queue) > self.nfiles_max:
        oldest = self.file_queue.popleft()
        self.delete(oldest)
