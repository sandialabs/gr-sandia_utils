/* -*- c++ -*- */
/*
 * Copyright 2018 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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

#ifndef @GUARD_NAME@
#define	@GUARD_NAME@

#include <sandia_utils/@BASE_NAME@.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace sandia_utils {

    class SANDIA_UTILS_API @IMPL_NAME@ : public @BASE_NAME@
    {
    protected:
      bool  d_flow_data;
      bool  d_consume_data;

    public:

      @IMPL_NAME@(bool flow_data, bool consume_data);
      virtual ~@IMPL_NAME@();

      void set_flow_data(bool flow_data);
      bool get_flow_data() const;

      void set_consume_data(bool consume_data);
      bool get_consume_data() const;

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* @GUARD_NAME@ */
