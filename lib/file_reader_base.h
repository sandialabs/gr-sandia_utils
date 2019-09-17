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

#ifndef INCLUDED_SANDIA_UTILS_FILE_READER_BASE_H
#define INCLUDED_SANDIA_UTILS_FILE_READER_BASE_H

#include <string>
#include <stdint.h>           /* uint64_t */
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <sandia_utils/api.h>
#include <pmt/pmt.h>
#include <gnuradio/logger.h>
#include <gnuradio/tags.h>
#include "epoch_time.h"

// win32 (mingw/msvc) specific
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define	OUR_O_BINARY O_BINARY
#else
#define	OUR_O_BINARY 0
#endif
// should be handled via configure
#ifdef O_LARGEFILE
#define	OUR_O_LARGEFILE	O_LARGEFILE
#else
#define	OUR_O_LARGEFILE 0
#endif

namespace gr {
  namespace sandia_utils {
    class SANDIA_UTILS_API file_reader_base
    {
    protected:
      // itemsize in bytes
      size_t            d_itemsize;

      // file status
      bool              d_is_open;
      std::string       d_filename;
      FILE              *d_fp;
      int               d_file_size;

      // metadata tags
      std::vector<gr::tag_t> d_tags;

      // logger
      gr::logger_ptr            d_logger;

    public:
      typedef boost::shared_ptr<file_reader_base> sptr;

      file_reader_base(size_t itemsize, gr::logger_ptr logger);
      ~file_reader_base();

      virtual void open(const char *filename);
      virtual bool is_open() { return d_is_open; }
      virtual void close();
      virtual int read(char *dest, int nitems);
      virtual std::vector<gr::tag_t> get_tags() {
       return d_tags;
      }
      virtual bool seek(long seek_point, int whence);
      virtual bool eof() {
        return (ftell(d_fp) == d_file_size);
      }
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_BASE_H */
