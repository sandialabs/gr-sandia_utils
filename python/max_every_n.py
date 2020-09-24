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

class max_every_n(gr.decim_block):
    """
    Max Every N

    Output the maximum input value for every N input samples
    """
    def __init__(self, n):
        gr.decim_block.__init__(self,
            name="max_every_n",
            in_sig=[numpy.float32],
            out_sig=[numpy.float32], decim=n)

        #self.set_decimation(n)
        self.n = n

    def work(self, input_items, output_items):
        in0 = input_items[0]
        out = output_items[0]

        nout = int(len(in0)/self.n)
        for ii in range(0,nout):
            out[ii] = in0[ii*self.n + numpy.argmax(numpy.abs(in0[ii*self.n:(ii+1)*self.n]))]

        return nout
