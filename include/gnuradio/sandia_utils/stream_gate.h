/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_STREAM_GATE_H
#define INCLUDED_SANDIA_UTILS_STREAM_GATE_H

#include <gnuradio/sandia_utils/api.h>
#include <gnuradio/sync_block.h>

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
    typedef std::shared_ptr<stream_gate<T>> sptr;

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
     * \param vlen            Input vector length
     */
    static sptr make(bool flow_data = true, bool consume_data = true, int vlen = 1);

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
