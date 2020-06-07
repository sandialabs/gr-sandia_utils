/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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
        // typedef boost::shared_ptr<file_reader_base> sptr;

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
