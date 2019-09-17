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

#ifndef INCLUDED_SANDIA_UTILS_INVERT_TUNE_IMPL_H
#define INCLUDED_SANDIA_UTILS_INVERT_TUNE_IMPL_H

#include <sandia_utils/invert_tune.h>

namespace gr {
  namespace sandia_utils {

    class invert_tune_impl : public invert_tune
    {
     private:
      uint64_t d_n_error;
      pmt::pmt_t d_mp_name;
      pmt::pmt_t d_freq_key;

     public:
      invert_tune_impl();
      ~invert_tune_impl();
      bool stop();

      void handle_msg(pmt::pmt_t);

    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INVERT_TUNE_IMPL_H */
