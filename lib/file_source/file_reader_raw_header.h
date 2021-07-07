/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_FILE_READER_RAW_HEADER_H
#define INCLUDED_SANDIA_UTILS_FILE_READER_RAW_HEADER_H

#include "file_reader_base.h"
#include "../epoch_time.h"

namespace gr
{
  namespace sandia_utils
  {
    class SANDIA_UTILS_API file_reader_raw_header : public file_reader_base
    {
      private:
        // nothing here
      public:
        // typedef std::shared_ptr<file_reader_base> sptr;

        file_reader_raw_header( size_t itemsize, gr::logger_ptr logger ) : file_reader_base( itemsize, logger )
        {
        }
        ~file_reader_raw_header()
        {
        }

        virtual void open( const char *filename );

    }; //end class file_reader_raw_header

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_RAW_HEADER_H */
