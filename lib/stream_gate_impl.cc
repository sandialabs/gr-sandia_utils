/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_gate_impl.h"
#include <gnuradio/io_signature.h>
#include <unistd.h> /* usleep */

namespace gr {
namespace sandia_utils {

template <class T>
typename stream_gate<T>::sptr
stream_gate<T>::make(bool flow_data, bool consume_data, int vlen)
{
    return gnuradio::get_initial_sptr(
        new stream_gate_impl<T>(flow_data, consume_data, vlen));
}

template <class T>
stream_gate_impl<T>::stream_gate_impl(bool flow_data, bool consume_data, int vlen)
    : gr::block("stream_gate",
                io_signature::make(1, 1, sizeof(T) * vlen),
                io_signature::make(1, 1, sizeof(T) * vlen)),
      d_dropped_samples(0),
      d_flow_data(flow_data),
      d_consume_data(consume_data),
      d_vlen(vlen)
{
    // tags are manually propagated to account for dropped data
    this->set_tag_propagation_policy(gr::block::TPP_DONT);
}

template <class T>
stream_gate_impl<T>::~stream_gate_impl()
{
    /* NOOP */
}

template <class T>
void stream_gate_impl<T>::forecast(int noutput_items,
                                   gr_vector_int& ninput_items_required)
{
    size_t ninputs = ninput_items_required.size();
    for (size_t i = 0; i < ninputs; i++) {
        ninput_items_required[i] = noutput_items;
    }
}

template <class T>
void stream_gate_impl<T>::set_flow_data(bool flow_data)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_flow_data = flow_data;
}

template <class T>
bool stream_gate_impl<T>::get_flow_data() const
{
    return d_flow_data;
}

template <class T>
void stream_gate_impl<T>::set_consume_data(bool consume_data)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_consume_data = consume_data;
}

template <class T>
bool stream_gate_impl<T>::get_consume_data() const
{
    return d_consume_data;
}


template <class T>
int stream_gate_impl<T>::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);


    T* in = (T*)input_items[0];
    T* out = (T*)output_items[0];

    if (d_flow_data) {
        // tags are manually propagated so the offset can be adjusted to accomodate
        // previously dropped data
        std::vector<tag_t> tags;
        this->get_tags_in_window(tags, 0, 0, noutput_items);
        for (auto tag : tags) {
            tag.offset -= d_dropped_samples;
            this->add_item_tag(0, tag);
        }

        // copy data to output and update scheduler
        memcpy(out, in, sizeof(T) * noutput_items * d_vlen);
        this->produce(0, noutput_items);
        this->consume_each(noutput_items);

    } else {
        // do not produce any items
        this->produce(0, 0);

        if (d_consume_data) {
            d_dropped_samples += noutput_items;
            this->consume_each(noutput_items);
        } else {
            // dont overload the processor
            usleep(1000);
        }
    }

    return gr::block::WORK_CALLED_PRODUCE;
}

template class stream_gate<unsigned char>;
template class stream_gate<short>;
template class stream_gate<int32_t>;
template class stream_gate<float>;
template class stream_gate<gr_complex>;
} // namespace sandia_utils
} /* namespace gr */
