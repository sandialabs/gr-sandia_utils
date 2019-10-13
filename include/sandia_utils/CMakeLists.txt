# Copyright 2011,2012 Free Software Foundation, Inc.
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
# Invoke macro to generate various headers
#######################################################################
include(GrMiscUtils)

add_custom_target(sandia_utils_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install public header files
########################################################################
install(FILES
    stream_gate_bb.h  
    stream_gate_ss.h
    stream_gate_cc.h  
    stream_gate_ff.h
    api.h
    file_sink.h
    burst_power_detector.h
    tag_debug_file.h
    invert_tune.h
    block_buffer.h
    message_vector_file_sink.h
    message_vector_raster_file_sink.h
    file_source.h
    constants.h
    interleaved_short_to_complex.h
    complex_to_interleaved_short.h DESTINATION include/sandia_utils
)
