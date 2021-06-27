# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT sandia_utils_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT sandia_utils_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND sandia_utils_python_files
    block_buffer_python.cc
    burst_power_detector_python.cc
    complex_to_interleaved_short_python.cc
    compute_stats_python.cc
    constants_python.cc
    file_sink_python.cc
    file_source_python.cc
    interleaved_short_to_complex_python.cc
    invert_tune_python.cc
    message_vector_file_sink_python.cc
    message_vector_raster_file_sink_python.cc
    sandia_tag_debug_python.cc
    stream_gate_python.cc
    tag_debug_file_python.cc
    tagged_bits_to_bytes_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(sandia_utils
   ../..
   gr::sandia_utils
   "${sandia_utils_python_files}")

install(TARGETS sandia_utils_python DESTINATION ${GR_PYTHON_DIR}/sandia_utils COMPONENT pythonapi)
