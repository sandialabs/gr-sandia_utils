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


 #ifndef @GUARD_NAME@
 #define	@GUARD_NAME@

#include <sandia_utils/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace sandia_utils {

    /*!
     * \brief Control streaming data with @I_TYPE@ input, @O_TYPE@ output
     *
     * This block controls the flow of streaming data.  When the block is not
     * flowing data, it can be set to either consume the data and drop it on
     * the floor or not to consume data, causing the system to be backpressured.
     *
     */
    class SANDIA_UTILS_API @BASE_NAME@ : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of sandia_utils::@BASE_NAME@.
       *
       * To avoid accidental use of raw pointers, sandia_utils::block_buffer's
       * constructor is in a private implementation
       * class. sandia_utils::@BASE_NAME@::make is the public interface for
       * creating new instances.
       *
       * \param flow_data       Flow data
       * \param consume_data    Consume data when not flowing
       */
      static sptr make(bool flow_data = true, bool consume_data=true);

      // set flag to flow data
      virtual void set_flow_data(bool flow_data) = 0;
      virtual bool get_flow_data() const = 0;

      // set flag to consume data when not flowing
      virtual void set_consume_data(bool consume_data) = 0;
      virtual bool get_consume_data() const = 0;
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* @GUARD_NAME */
