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

########################################################################
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
GR_PYTHON_INSTALL(
    FILES
    __init__.py
    uhd_timed_cordic_emulator.py
    message_file_debug.py
    python_interface_sink.py
    python_message_interface.py
    file_archiver.py
    csv_reader.py
    csv_writer.py
    rftap_encap.py DESTINATION ${GR_PYTHON_DIR}/sandia_utils
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-sandia_utils)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig)
GR_ADD_TEST(qa_tag_debug_file ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tag_debug_file.py)
GR_ADD_TEST(qa_invert_tune ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_invert_tune.py)
GR_ADD_TEST(qa_uhd_timed_cordic_emulator ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_uhd_timed_cordic_emulator.py)
GR_ADD_TEST(qa_message_vector_file_sink ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_message_vector_file_sink.py)
GR_ADD_TEST(qa_message_vector_raster_file_sink ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_message_vector_raster_file_sink.py)
GR_ADD_TEST(qa_file_source ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_file_source.py)
#GR_ADD_TEST(qa_file_source ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_file_sink.py)
GR_ADD_TEST(qa_interleaved_short_to_complex ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_interleaved_short_to_complex.py)
GR_ADD_TEST(qa_file_archiver ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_file_archiver.py)
GR_ADD_TEST(qa_complex_to_interleaved_short ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_complex_to_interleaved_short.py)
GR_ADD_TEST(qa_tagged_bits_to_bytes ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tagged_bits_to_bytes.py)
GR_ADD_TEST(qa_csv_reader ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_csv_reader.py)
GR_ADD_TEST(qa_csv_writer ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_csv_writer.py)
GR_ADD_TEST(qa_rftap_encap ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_rftap_encap.py)
