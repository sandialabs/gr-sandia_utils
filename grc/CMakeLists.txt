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
configure_file(sandia_utils_file_sink.xml ${CMAKE_BINARY_DIR}/grc/)
configure_file(sandia_utils_file_source.xml ${CMAKE_BINARY_DIR}/grc/)
install(FILES
    ${CMAKE_BINARY_DIR}/grc/sandia_utils_file_sink.xml
    ${CMAKE_BINARY_DIR}/grc/sandia_utils_file_source.xml 
    sandia_utils_burst_power_detector.xml
    sandia_utils_tag_debug_file.xml
    sandia_utils_invert_tune.xml
    sandia_utils_uhd_timed_cordic_emulator.xml
    sandia_utils_message_file_debug.xml
    sandia_utils_block_buffer.xml
    sandia_utils_python_interface_sink.xml
    sandia_utils_python_message_interface.xml
    sandia_utils_message_vector_file_sink.xml
    sandia_utils_message_vector_raster_file_sink.xml
    sandia_utils_stream_gate_XX.xml
    sandia_utils_interleaved_short_to_complex.xml
    sandia_utils_file_monitor.xml
    sandia_utils_complex_to_interleaved_short.xml DESTINATION share/gnuradio/grc/blocks
)
