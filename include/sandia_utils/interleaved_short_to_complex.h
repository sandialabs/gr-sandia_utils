/* -*- c++ -*- */
/* 
 * Copyright 2019 gr-sandia_utils author.
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


#ifndef INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_H
#define INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_H

#include <sandia_utils/api.h>
#include <gnuradio/sync_decimator.h>

namespace gr {
  namespace sandia_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup sandia_utils
     *
     */
    class SANDIA_UTILS_API interleaved_short_to_complex : virtual public gr::sync_decimator
    {
     public:
      typedef boost::shared_ptr<interleaved_short_to_complex> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of sandia_utils::interleaved_short_to_complex.
       *
       * To avoid accidental use of raw pointers, sandia_utils::interleaved_short_to_complex's
       * constructor is in a private implementation
       * class. sandia_utils::interleaved_short_to_complex::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool vector_input=false, bool swap=false, float scale=1.0);

      virtual void set_swap(bool swap)=0;
      virtual void set_scale(float scale)=0;
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INTERLEAVED_SHORT_TO_COMPLEX_H */

