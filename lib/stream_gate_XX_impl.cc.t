/* -*- c++ -*- */
/*
 * <COPYRIGHT PLACEHOLDER>
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


/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "@IMPL_NAME@.h"
#include <gnuradio/io_signature.h>
#include <unistd.h> /* usleep */

namespace gr {
  namespace sandia_utils {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(bool flow_data,
                      bool consume_data)
    {
      return gnuradio::get_initial_sptr(new @IMPL_NAME@
					(flow_data,consume_data));
    }

    @IMPL_NAME@::@IMPL_NAME@(bool flow_data,
                             bool consume_data)
    : gr::block("@BASE_NAME@",
			io_signature::make(1, 1, sizeof(@I_TYPE@)),
			io_signature::make(1, 1, sizeof(@O_TYPE@))),
      d_flow_data(flow_data),
      d_consume_data(consume_data)
    {
      /* NOOP */
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
      /* NOOP */
    }

    void
    @IMPL_NAME@::set_flow_data(bool flow_data)
    {
      gr::thread::scoped_lock l(d_setlock);
      d_flow_data = flow_data;
    }

    bool
    @IMPL_NAME@::get_flow_data() const
    {
      return d_flow_data;
    }

    void
    @IMPL_NAME@::set_consume_data(bool consume_data)
    {
      gr::thread::scoped_lock l(d_setlock);
      d_consume_data = consume_data;
    }

    bool
    @IMPL_NAME@::get_consume_data() const
    {
      return d_consume_data;
    }


    int
    @IMPL_NAME@::general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_setlock);

      @I_TYPE@ *in  = (@I_TYPE@ *)input_items[0];
      @O_TYPE@ *out = (@O_TYPE@ *)output_items[0];

      if (d_flow_data) {
        // copy data over
        memcpy(out,in,sizeof(@I_TYPE@)*noutput_items);
        consume_each(noutput_items);

        // signal that samples were produced
        produce(0,noutput_items);
      }
      else {
        // should we consume data and drop it on the floor or just
        // block and allow backpressuring?
        if (d_consume_data) {
          consume_each(noutput_items);
          // std::cout << "consumed " << noutput_items << " items " << std::endl;
        }
        else {
          // dont overload the processor
          usleep(1000);
        }

        // do not produce any items
        produce(0,0);
      }

      return WORK_CALLED_PRODUCE;
    }

  } /* namespace timing_utils */
} /* namespace gr */
