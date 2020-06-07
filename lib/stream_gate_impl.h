/* -*- c++ -*- */
/*
 * Copyright 2020 gr-sandia_utils author.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SANDIA_UTILS_STREAM_GATE_IMPL_H
#define INCLUDED_SANDIA_UTILS_STREAM_GATE_IMPL_H

#include <sandia_utils/stream_gate.h>

#include <gnuradio/io_signature.h>
#include <sandia_utils/stream_gate.h>

namespace gr {
namespace sandia_utils {

template <class T>
class SANDIA_UTILS_API stream_gate_impl : public stream_gate<T>
{
protected:
    bool d_flow_data;
    bool d_consume_data;

public:
    /**
     * Constructor
     *
     * @param flow_data - true to flow data through block
     * @param consume_data - true to consume incoming data when not flowing
     */
    stream_gate_impl(bool flow_data, bool consume_data);

    /**
     * Deconstructor
     */
    ~stream_gate_impl();

    /*!
     * \brief Flow data
     *
     * @param flow_data - true to flow data
     */
    void set_flow_data(bool flow_data);

    /*!
     * \brief Get gate status
     *
     * @return bool - true when flowing data
     */
    bool get_flow_data() const;

    /*!
     * \brief Consume data when not flowing
     *
     * @param consume_data - true to consume data when not flowing
     */
    void set_consume_data(bool consume_data);

    /*!
     * \brief Get consumption status
     *
     * @return bool - true when consuming data
     */
    bool get_consume_data() const;

    /**
     * process incoming stream
     *
     * @param noutput_items
     * @param ninput_items
     * @param input_items
     * @param output_items
     * @return int
     */
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_STREAM_GATE_IMPL_H */
