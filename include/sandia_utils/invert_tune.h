/* -*- c++ -*- */
/* 
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_SANDIA_UTILS_INVERT_TUNE_H
#define INCLUDED_SANDIA_UTILS_INVERT_TUNE_H

#include <sandia_utils/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace sandia_utils {

    /*!
     * \brief <+description of block+>
     * \ingroup sandia_utils
     *
     */
    class SANDIA_UTILS_API invert_tune : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<invert_tune> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of sandia_utils::invert_tune.
       *
       * To avoid accidental use of raw pointers, sandia_utils::invert_tune's
       * constructor is in a private implementation
       * class. sandia_utils::invert_tune::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INVERT_TUNE_H */

