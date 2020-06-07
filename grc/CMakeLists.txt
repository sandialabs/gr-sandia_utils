# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.


MESSAGE(STATUS "Source Dir: ${CMAKE_BINARY_DIR}")
install(FILES
    sandia_utils_file_sink.block.yml
    sandia_utils_file_source.block.yml
    sandia_utils_burst_power_detector.block.yml
    sandia_utils_message_file_debug.block.yml
    sandia_utils_block_buffer.block.yml
    sandia_utils_python_interface_sink.block.yml
    sandia_utils_python_message_interface.block.yml
    sandia_utils_max_every_n.block.yml
    sandia_utils_message_vector_file_sink.block.yml
    sandia_utils_message_vector_raster_file_sink.block.yml
    sandia_utils_interleaved_short_to_complex.block.yml
    sandia_utils_file_archiver.block.yml
    sandia_utils_complex_to_interleaved_short.block.yml
    sandia_utils_csv_reader.block.yml
    sandia_utils_csv_writer.block.yml
    sandia_utils_rftap_encap.block.yml
    sandia_utils_invert_tune.block.yml
    sandia_utils_rftap_encap.block.yml
    sandia_utils_stream_gate.block.yml
    sandia_utils_tag_debug_file.block.yml
    sandia_utils_tagged_bits_to_bytes.block.yml
    sandia_utils_uhd_timed_cordic_emulator.block.yml
    sandia_utils_compute_stats.block.yml DESTINATION share/gnuradio/grc/blocks
)
