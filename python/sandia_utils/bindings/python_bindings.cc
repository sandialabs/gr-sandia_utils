/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
void bind_block_buffer(py::module& m);
void bind_burst_power_detector(py::module& m);
void bind_compute_stats(py::module& m);
void bind_constants(py::module& m);
void bind_file_sink(py::module& m);
void bind_file_source(py::module& m);
void bind_invert_tune(py::module& m);
void bind_message_vector_file_sink(py::module& m);
void bind_message_vector_raster_file_sink(py::module& m);
void bind_sandia_tag_debug(py::module& m);
void bind_stream_gate(py::module& m);
void bind_tag_debug_file(py::module& m);
void bind_tagged_bits_to_bytes(py::module& m);
void bind_vita49_tcp_msg_source(py::module& m);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(sandia_utils_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_block_buffer(m);
    bind_burst_power_detector(m);
    bind_compute_stats(m);
    bind_constants(m);
    bind_file_sink(m);
    bind_file_source(m);
    bind_invert_tune(m);
    bind_message_vector_file_sink(m);
    bind_message_vector_raster_file_sink(m);
    bind_sandia_tag_debug(m);
    bind_stream_gate(m);
    bind_tag_debug_file(m);
    bind_tagged_bits_to_bytes(m);
    bind_vita49_tcp_msg_source(m);
    // ) END BINDING_FUNCTION_CALLS
}
