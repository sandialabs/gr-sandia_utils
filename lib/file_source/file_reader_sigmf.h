/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_FILE_READER_SIGMF_H
#define INCLUDED_SANDIA_UTILS_FILE_READER_SIGMF_H

#include "file_reader_base.h"
#include "../epoch_time.h"

namespace gr
{
  namespace sandia_utils
  {
    /**
     * Implements a File Reader for the SigMF format
     *
     * A SigMF recording is a pair of files that share a base name: the samples
     * live in <base>.sigmf-data and the metadata in <base>.sigmf-meta.  Either
     * member of the pair - or the base name alone - may be specified when
     * opening; any extension is stripped, the samples are always read from
     * <base>.sigmf-data and the tags are generated from <base>.sigmf-meta.
     *
     * Tags are generated from the first capture segment only, as the file
     * source applies tags at the first sample of the file.  The sample rate is
     * tagged as both rate and rx_rate.
     */
    class SANDIA_UTILS_API file_reader_sigmf : public file_reader_base
    {
      private:
        /**
         * Read the metadata file and populate the tags vector
         *
         * @param filename - name of the .sigmf-meta file to read
         */
        void parse_metadata( const std::string &filename );

      public:
        /**
         * Constructor
         *
         * @param itemsize - per item size in bytes
         * @param logger - parent file source logger instance
         */
        file_reader_sigmf( size_t itemsize, gr::logger_ptr logger ) : file_reader_base( itemsize, logger )
        {
        }
        ~file_reader_sigmf()
        {
        }

        virtual void open( const char *filename );

    }; //end class file_reader_sigmf

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_SIGMF_H */
