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
  Monitor file generation.

  Files can be either be moved to another location (i.e. ram to hard-disk) or
  when a certain number of files have accumlated, removed from the system.
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
        print('number of items to copy: ',len(self.copy_queue))

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
          print("generating output path {}".format(path))
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
          print("copying file {} to {}".format(fname,out_file))
          shutil.copy(fname,out_file)
        except Exception as e:
          print("unable to copy file: {}".format(e))

      # TODO: Make this dynamic prevent thread lock
      if not len(self.copy_queue):
        time.sleep(.5)

  def delete(self,fname):
    try:
      print("removing file {}".format(fname))
      os.remove(fname)
    except Exception as e:
      print("unable to delete file: {}".format(e))

  def handler(self, msg):
    # is pair() will pass both dictionaries and pairs
    if not pmt.is_pair(msg):
      return

    try:
      # will fail if msg is not a dictionary or pdu
      fpmt = pmt.dict_ref(msg,self.filename_tag,pmt.PMT_NIL)
    except:
      return

    if pmt.equal(fpmt, pmt.PMT_NIL):
      return
    fname = pmt.symbol_to_string(fpmt)
    print("received file {}".format(fname))

    with self.mutex:
      self.file_queue.append(fname)

      if self.copy:
        self.copy_queue.append(fname)

      if len(self.file_queue) > self.nfiles_max:
        oldest = self.file_queue.popleft()
        self.delete(oldest)
