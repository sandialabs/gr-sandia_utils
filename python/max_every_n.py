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
