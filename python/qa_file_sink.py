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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import sandia_utils_swig as sandia_utils

class qa_file_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiation (self):
        sink = sandia_utils.file_sink("complex", gr.sizeof_gr_complex*1, 'raw', sandia_utils.MANUAL, 0, int(1), "", "%Y%m%d_%H_%M_%S_fc=%fcMHz_fs=%fskHz.dat")
        sink.set_recording(False)
        sink.set_gen_new_folder(True)
        sink.set_second_align(True)
        sink.set_file_num_rollover(0)

        self.assertTrue(True)


if __name__ == '__main__':
    gr_unittest.run(qa_file_sink)
