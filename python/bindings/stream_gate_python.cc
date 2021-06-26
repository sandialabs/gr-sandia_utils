/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(stream_gate.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(319ecf9669abd63365872a056fb154fa)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <sandia_utils/stream_gate.h>
// pydoc.h is automatically generated in the build directory
#include <stream_gate_pydoc.h>

template <typename T>
void bind_stream_gate_template(py::module& m, const char* classname)
{
    using stream_gate = gr::sandia_utils::stream_gate<T>;

    py::class_<stream_gate,
               gr::block,
               gr::basic_block,
               std::shared_ptr<stream_gate>>(m, classname)

        .def(py::init(&gr::sandia_utils::stream_gate<T>::make),
           py::arg("flow_data") =  true,
           py::arg("consume_data") =  true,
           D(stream_gate,make)
        );
}

void bind_stream_gate(py::module& m)
{
    bind_stream_gate_template<unsigned char>(m, "stream_gate_b");
    bind_stream_gate_template<short>(m, "stream_gate_s");
    bind_stream_gate_template<int32_t>(m, "stream_gate_i");
    bind_stream_gate_template<float>(m, "stream_gate_f");
    bind_stream_gate_template<gr_complex>(m, "stream_gate_c");
}







