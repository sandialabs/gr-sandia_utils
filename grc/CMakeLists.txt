# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Configure files
########################################################################
configure_file(
    sandia_utils_file_sink.block.yml.in
    ${CMAKE_BINARY_DIR}/grc/sandia_utils_file_sink.block.yml
@ONLY)

configure_file(
    sandia_utils_file_source.block.yml.in
    ${CMAKE_BINARY_DIR}/grc/sandia_utils_file_source.block.yml
@ONLY)

MESSAGE(STATUS "Source Dir: ${CMAKE_BINARY_DIR}")
install(FILES
    ${CMAKE_BINARY_DIR}/grc/sandia_utils_file_sink.block.yml
    ${CMAKE_BINARY_DIR}/grc/sandia_utils_file_source.block.yml
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
    sandia_utils_tag_debug.block.yml
    sandia_utils_tagged_bits_to_bytes.block.yml
    sandia_utils_compute_stats.block.yml 
    sandia_utils_vita49_tcp_msg_source.block.yml
    sandia_utils_sigmf_meta_writer.block.yml DESTINATION share/gnuradio/grc/blocks
)
