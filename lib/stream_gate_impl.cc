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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_gate_impl.h"
#include <gnuradio/io_signature.h>
#include <unistd.h> /* usleep */

namespace gr {
namespace sandia_utils {

template <class T>
typename stream_gate<T>::sptr stream_gate<T>::make(bool flow_data, bool consume_data)
{
    return gnuradio::get_initial_sptr(new stream_gate_impl<T>(flow_data, consume_data));
}

template <class T>
stream_gate_impl<T>::stream_gate_impl(bool flow_data, bool consume_data)
    : gr::block("stream_gate",
                io_signature::make(1, 1, sizeof(T)),
                io_signature::make(1, 1, sizeof(T))),
      d_flow_data(flow_data),
      d_consume_data(consume_data)
{
    /* NOOP */
}

template <class T>
stream_gate_impl<T>::~stream_gate_impl()
{
    /* NOOP */
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
        // copy data over
        memcpy(out, in, sizeof(T) * noutput_items);
        this->consume_each(noutput_items);

        // signal that samples were produced
        this->produce(0, noutput_items);
    } else {
        // should we consume data and drop it on the floor or just
        // block and allow backpressuring?
        if (d_consume_data) {
            this->consume_each(noutput_items);
            // std::cout << "consumed " << noutput_items << " items " << std::endl;
        } else {
            // dont overload the processor
            usleep(1000);
        }

        // do not produce any items
        this->produce(0, 0);
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
