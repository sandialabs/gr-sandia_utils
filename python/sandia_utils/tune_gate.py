#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2022 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import numpy as np
import pmt
import time
from gnuradio import gr
import math
try:
    from gnuradio import sandia_utils
except ImportError:
    import os
    import sys
    dirname, filename = os.path.split(os.path.abspath(__file__))
    sys.path.append(os.path.join(dirname, "bindings"))
    from gnuradio import sandia_utils


class tune_gate(gr.basic_block):
    """
    Publish samples based on stream tags. A "tune_request" tag will
    discard a user-defined number of samples (to account for the delay
    between the tune request message and the actual tuning itself),
    and then publish the next samples to the output stream. If a
    "publish" tag is found, the publishing will start immediately.
    If a tag is not found after a defined timeout period, the block
    will automatically publish. The block will append tags that
    keep track of the offset relative to the input, and the event type.

    Args:
        n_pub - number of samples to output per publish
        n_discard - number of samples to discard after a tune request
        n_timeout - number of samples to wait for a tag before
                    automatically publishing
        samp_rate - sample rate
        datatype - input/output datatype (numpy.complex64, numpy.float32,
                   numpy.uint32, numpy.uint16, numpy.uint8)
    """

    def __init__(self, n_pub, n_discard, n_timeout, samp_rate, data_type):
        gr.basic_block.__init__(
            self, name="tune_gate",
            in_sig=[data_type],
            out_sig=[data_type])

        self.n_pub = n_pub
        self.n_discard = n_discard
        self.n_timeout = n_timeout

        # tag keys
        self.tune_key = sandia_utils.PMTCONSTSTR__tune_request()
        self.pub_key = sandia_utils.PMTCONSTSTR__publish()
        self.timeout_key = sandia_utils.PMTCONSTSTR__timeout()
        self.rx_time_key = sandia_utils.PMTCONSTSTR__rx_time()
        self.rx_rate_key = sandia_utils.PMTCONSTSTR__rx_rate()

        # store the output offset for tagging
        self.out_offset = 0

        self.samp_rate = samp_rate
        self.sleep_time = 1.0 / samp_rate

        # initialize tags
        self.tags = []
        self.pub_tags = []
        self.event_tag = None

        # cumulative RX time delay
        self.rx_delay = 0.0

        # state of block (wait for next tag, discard, or publish)
        self.state = sandia_utils.GATE_WAIT

        # number of samples read for current work call
        self.nread = 0
        # number of samples since last event
        self.n_since = 0
        # length of input
        self.in_len = 0
        self.out_len = 0

        # input samples
        self.input = []
        # output buffer
        self.out_buf = []

        self.pub_offset = 0

        self.nout = 0
        self.nout_total = 0

        # if the block is in the middle of publishing after a timeout
        self.pubtimeout = False

        # don't automatically propagate tags
        self.set_tag_propagation_policy(gr.TPP_DONT)

    def reset():
        self.rx_delay = 0.0

    def set_n_discard(self, n_discard):
        self.n_discard = n_discard

    # attempt to discard samples
    # if there are enough, discard them and return true
    # else, discard as many as possible, update state and return false
    def discard(self):
        if self.state == sandia_utils.GATE_WAIT:
            self.n_since = 0

        # search for any rx_rate_tags and update the cumulative delay / sample rate sequentially
        idx = 0
        t = 0.0
        n = 0
        samps_left = np.min([self.n_discard - self.n_since, self.in_len - self.nread])
        tags_read = 0
        for tag in self.tags:
            if tag.offset - self.nread < samps_left + self.nitems_read(0):
                tags_read += 1
                if pmt.eqv(tag.key, self.rx_rate_key):
                    n = tag.offset - self.nread - idx
                    t += n / self.samp_rate
                    self.samp_rate = pmt.to_double(tag.value)
                    idx = tag.offset - self.nread
        self.tags = self.tags[tags_read:]
        n = samps_left - idx
        t += n / self.samp_rate
        self.update_rx_time_tags(t)
        self.rx_delay += t

        if self.n_discard - self.n_since > self.in_len - self.nread:
            self.n_since += self.in_len - self.nread
            self.nread = self.in_len
            self.state = sandia_utils.GATE_DISCARD
            return False
        else:
            self.nread += self.n_discard - self.n_since
            self.n_since = 0
            self.state = sandia_utils.GATE_WAIT
            return True

    # attempt to publish
    def publish(self):
        if self.state == sandia_utils.GATE_WAIT:
            self.n_since = 0
        samps_left = np.min([self.n_pub - self.n_since, self.in_len - self.nread])
        tags_read = 0
        for tag in self.tags:
            if tag.offset - self.nread < self.n_pub + self.nitems_read(0):
                tags_read += 1
                if pmt.eqv(tag.key, self.rx_rate_key):
                    self.samp_rate = pmt.to_double(tag.value)
                    self.add_item_tag(0, self.nout_total + self.nout - self.nread + tag.offset - self.nitems_read(0), tag.key, tag.value, tag.srcid)
                elif pmt.eqv(tag.key, self.pub_key):
                    pass
                elif pmt.eqv(tag.key, self.tune_key):
                    pass
                else:
                    self.add_item_tag(0, self.nout_total + self.nout - self.nread + tag.offset - self.nitems_read(0), tag.key, tag.value, tag.srcid)
        self.tags = self.tags[tags_read:]
        # if there aren't enough samples, read in as many samples as possible
        # and update state to mid-publish, then return false
        if self.n_since + self.in_len - self.nread < self.n_pub:
            self.n_since += self.in_len - self.nread
            self.out_buf.extend(self.input[self.nread:])
            self.nread = self.in_len
            self.state = sandia_utils.GATE_PUBLISH
            return False
        # otherwise fill buffer and update state to "wait"
        else:
            self.out_buf.extend(self.input[self.nread:(self.nread + self.n_pub - len(self.out_buf))])
            self.nread += self.n_pub - self.n_since
            self.n_since = 0
            self.state = sandia_utils.GATE_WAIT
            return True

    def update_rx_time_tags(self, delay):
        for tag in self.tags:
            if pmt.eqv(tag.key, self.rx_time_key):
                if pmt.is_pair(tag.value):
                    tag_time = pmt.to_uint64(pmt.car(tag.value)) + pmt.to_double(pmt.cdr(tag.value))
                    tag_time += delay
                    new_tag_sec = math.floor(tag_time)
                    new_tag_frac = tag_time % 1
                    tag.value = pmt.cons(pmt.from_uint64(new_tag_sec), pmt.from_double(new_tag_frac))
                else:
                    tag.value = pmt.from_double(pmt.to_double(tag.value) + delay)

    def general_work(self, input_items, output_items):
        # handle input, size of input and size of output
        self.input = input_items[0]
        self.in_len = len(self.input)
        self.out_len = len(output_items[0])

        self.sleep_time = self.in_len / 2.0 / self.samp_rate

        # re-initialize number of output samples and number of samples read
        self.nout = 0
        self.nread = 0

        # re-initialize the fill output buffer boolean to false
        fill_out_buf = False

        # read in all tags for current input
        self.tags = self.get_tags_in_window(0, 0, self.in_len)
        # update rx_time tags with cumulative delay
        self.update_rx_time_tags(self.rx_delay)
        # while loop to loop through input data
        while self.nread < self.in_len:
          # if block is in wait mode
            if len(self.tags) > 0:
                next_tag = self.tags[0].offset - self.nitems_read(0)
            else:
                next_tag = self.in_len + self.n_pub + 1

            if self.state == sandia_utils.GATE_WAIT:
                next_eob = self.in_len
                next_timeout = self.n_timeout - self.n_since + self.nread

                next_event = np.argmin([next_tag, next_timeout, next_eob])
                # next event = tag
                if next_event == 0:
                    # tune tag
                    if pmt.eqv(self.tags[0].key, self.tune_key):
                        #self.rx_delay += self.n_discard / self.samp_rate
                        #print("tune tag found, self.rx_delay =",self.rx_delay)
                        self.event_tag = gr.tag_utils.python_to_tag(
                            (self.out_offset, self.tune_key, pmt.from_long(self.tags[0].offset), pmt.intern("src")))
                        self.nread = next_tag
                        # check if there are enough samples to discard
                        if self.discard() and self.publish():
                            fill_out_buf = True

                    # publish tag
                    elif pmt.eqv(self.tags[0].key, self.pub_key):
                        self.event_tag = gr.tag_utils.python_to_tag(
                            (self.out_offset, self.pub_key, pmt.from_long(self.tags[0].offset), pmt.intern("src")))
                        self.nread = next_tag
                        # check if there are enough samples to publish
                        if self.publish():
                            fill_out_buf = True

                    # next event = update sample rate
                    elif pmt.eqv(self.tags[0].key, self.rx_rate_key):
                        self.samp_rate = pmt.to_double(self.tags[0].value)
                        self.n_since += next_tag - self.nread
                        self.nread = next_tag
                        self.tags = self.tags[1:]

                    else:
                        self.n_since += next_tag
                        self.nread = next_tag
                        self.tags = self.tags[1:]

                # next event = potential timeout
                elif next_event == 1:
                    self.nread += self.n_timeout - self.n_since
                    if next_tag < self.nread + self.n_pub:
                        self.nread = next_tag
                    else:
                        self.event_tag = gr.tag_utils.python_to_tag(
                            (self.out_offset, self.timeout_key, pmt.from_long(self.nitems_read(0) + self.nread), pmt.intern("gate")))
                        self.pubtimeout = True
                        if self.publish():
                            fill_out_buf = True

                # next event = end of block
                elif next_event == 2:
                    self.n_since += self.in_len - self.nread
                    self.nread = self.in_len
                    time.sleep(self.sleep_time)

            # if block is in discard mode
            elif self.state == sandia_utils.GATE_DISCARD:
                # check if there are enough samples to discard
                if self.discard() and self.publish():
                    fill_out_buf = True

            # if block is in publish mode
            elif self.state == sandia_utils.GATE_PUBLISH:
                if self.n_pub - self.n_since + self.nread > next_tag - self.nitems_read(0) and self.pubtimeout:
                    self.nread = next_tag
                    self.out_buf = []
                    self.state = sandia_utils.GATE_WAIT
                    self.pubtimeout = False
                    self.n_since = 0
                elif self.publish():
                    fill_out_buf = True

            if fill_out_buf:
                if len(self.out_buf) > len(output_items[0]):
                    output_items[0][:] = self.out_buf[:self.out_len]
                    del self.out_buf[:self.out_len]
                    self.state = sandia_utils.GATE_PUBLISH
                    self.out_offset += len(output_items[0])
                    self.consume_each(self.nread)
                    return len(output_items[0])
                else:
                    self.nout += self.n_pub
                    output_items[0][(self.nout - self.n_pub):self.nout] = self.out_buf
                    self.out_buf = []
                    self.add_item_tag(0, self.event_tag)
                    self.out_offset += self.n_pub
                    if self.nout + self.n_pub > len(output_items[0]):
                        self.consume_each(self.nread)
                        return self.nout
                fill_out_buf = False

        self.nout_total += self.nout
        self.consume_each(self.in_len)
        self.tags = []
        return self.nout
