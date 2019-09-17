/* -*- c++ -*- */
/*
 * Copyright 2017 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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

#include "file_writer_raw.h"

#include <iostream>
#include <stdio.h>
#include <boost/format.hpp>


namespace gr {
  namespace sandia_utils {
    file_writer_raw::file_writer_raw(std::string data_type, std::string file_type,
                                 size_t itemsize, uint64_t nsamples, int rate,
                                 std::string out_dir, std::string name_spec)
    {
      /* NOOP */
    }

    file_writer_raw::~file_writer_raw()
    {
      // ensure file descriptor is closed
      close();
    }

    void
    file_writer_raw::open(std::string fname){
      GR_LOG_DEBUG(d_logger,boost::format("Opening file %s") % fname.c_str());
      d_outfile.open(fname.c_str(), std::ofstream::binary);
    }

    void
    file_writer_raw::close(){
      if (d_outfile.is_open()) {
        GR_LOG_DEBUG(d_logger,boost::format("Closing file %s") % d_filename);
        d_outfile.close();
        }
    }

    int
    file_writer_raw::write_impl(const void *in, int nitems){
      d_outfile.write((const char *)in,nitems*d_itemsize);
      return nitems;
    }

  } /* namespace sandia_utils */
} /* namespace gr */
