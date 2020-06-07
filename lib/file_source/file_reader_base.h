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
#include "../epoch_time.h"

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

namespace gr
{
  namespace sandia_utils
  {
    /**
     * Base Class for file source reader.
     * Extending classes implement specific file formats.
     * Base implementation implements Raw IQ file format
     */
    class SANDIA_UTILS_API file_reader_base
    {
      protected:
        // itemsize in bytes
        size_t d_itemsize;

        // file status
        bool d_is_open;
        std::string d_filename;
        FILE *d_fp;
        int d_file_size;

        // metadata tags
        std::vector<gr::tag_t> d_tags;

        // logger
        gr::logger_ptr d_logger;

      public:
        typedef boost::shared_ptr<file_reader_base> sptr;

        /**
         * Consructor
         *
         * @param itemsize - per item size in bytes
         * @param logger - parent file source logger instance
         */
        file_reader_base( size_t itemsize, gr::logger_ptr logger );

        /**
         * Deconstructor
         */
        virtual ~file_reader_base();

        /**
         * Opens a new file. Closes current file if open
         *
         * @param filename - filename of file to open
         */
        virtual void open( const char *filename );

        /**
         * returns file open status
         *
         * @return bool - true if reader is open
         */
        virtual bool is_open()
        {
          return d_is_open;
        }

        /**
         * Closes the open file
         */
        virtual void close();

        /**
         * Read items from file.
         * Number of bytes read from file is based on #d_itemsize
         *
         * @param dest - destination storage for sample
         * @param nitems - number of items to ready
         * @return int - number of items read. 0 on EOF or error
         */
        virtual int read( char *dest, int nitems );

        /**
         * Returns tags vector
         *
         * @return vector<gr::tag_g> - tags vector
         */
        virtual std::vector<gr::tag_t> get_tags()
        {
          return d_tags;
        }

        /**
         * Seek in the file source
         *
         * @param seek_point - offset position to seek to
         * @param whence - refrence point for seek, see fseek
         * @return bool - 0 or false on success
         */
        virtual bool seek( long seek_point, int whence );

        /**
         * Returns End Of File( EOF ) status
         *
         * @return bool - true if EOF
         */
        virtual bool eof()
        {
          return (ftell( d_fp ) == d_file_size);
        }
    }; //end class file_reader_base

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_BASE_H */
