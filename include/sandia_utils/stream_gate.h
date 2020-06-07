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

#ifndef INCLUDED_SANDIA_UTILS_STREAM_GATE_H
#define INCLUDED_SANDIA_UTILS_STREAM_GATE_H

#include <gnuradio/sync_block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Control the streaming of data
 * \ingroup sandia_utils
 *
 * Control the flow of streaming data.  Data flow can be started and stopped.  When
 * stopped, the block can either consume data from the input stream and not produce
 * any data, or it can not consume or produce data, electing to sleep to reduce
 * processing load.  The former condition is necessary when two blocks are
 * connected to the same input buffer and one is flowing data while the other is
 * not.
 */
template <class T>
class SANDIA_UTILS_API stream_gate : virtual public gr::block
{
public:
    typedef boost::shared_ptr<stream_gate<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::stream_gate.
     *
     * To avoid accidental use of raw pointers, sandia_utils::stream_gate's
     * constructor is in a private implementation
     * class. sandia_utils::stream_gate::make is the public interface for
     * creating new instances.
     *
     * \param flow_data       Flow data
     * \param consume_data    Consume data when not flowing
     */
    static sptr make(bool flow_data = true, bool consume_data = true);

    /*!
     * \brief Flow data
     *
     * @param flow_data - true to flow data
     */
    virtual void set_flow_data(bool flow_data) = 0;

    /*!
     * \brief Get gate status
     *
     * @return bool - true when flowing data
     */
    virtual bool get_flow_data() const = 0;

    /*!
     * \brief Consume data when not flowing
     *
     * @param consume_data - true to consume data when not flowing
     */
    virtual void set_consume_data(bool consume_data) = 0;

    /*!
     * \brief Get consumption status
     *
     * @return bool - true when consuming data
     */
    virtual bool get_consume_data() const = 0;
};

typedef stream_gate<unsigned char> stream_gate_b;
typedef stream_gate<short> stream_gate_s;
typedef stream_gate<int32_t> stream_gate_i;
typedef stream_gate<float> stream_gate_f;
typedef stream_gate<gr_complex> stream_gate_c;
} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_STREAM_GATE_H */
