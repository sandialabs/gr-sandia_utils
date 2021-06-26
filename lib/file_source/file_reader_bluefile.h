/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
        // typedef std::shared_ptr<file_reader_base> sptr;

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
