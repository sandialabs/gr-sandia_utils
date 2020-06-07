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

#ifndef INCLUDED_SANDIA_UTILS_FILE_READER_BLUEFILE_H
#define INCLUDED_SANDIA_UTILS_FILE_READER_BLUEFILE_H

#include "file_reader_base.h"
#include "../epoch_time.h"

#include "bluefile.hh"

namespace gr
{
  namespace sandia_utils
  {
    /**
     * Implements a File Reater for the bluefile format
     */
    class SANDIA_UTILS_API file_reader_bluefile : public file_reader_base
    {
      private:
        bluefile::BlueFile *d_blue_reader;
        std::string d_type;
        int d_bpe;
        int d_bps;

      public:
        // typedef boost::shared_ptr<file_reader_base> sptr;

        /**
         * Consructor
         *
         * @param itemsize - per item size in bytes
         * @param logger - parent file source logger instance
         */
        file_reader_bluefile( size_t itemsize, gr::logger_ptr logger ) : file_reader_base( itemsize, logger )
        {
        }
        ~file_reader_bluefile()
        {
        }

        virtual void open( const char *filename );

        virtual int read( char *dest, int nitems );

        virtual bool seek( long seek_point, int whence );

        virtual void close();

        virtual bool eof();

    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_BLUEFILE_H */
