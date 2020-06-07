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

#ifndef INCLUDED_SANDIA_UTILS_FILE_WRITER_BLUEFILE_H
#define INCLUDED_SANDIA_UTILS_FILE_WRITER_BLUEFILE_H

#include <fstream>
#include <sandia_utils/api.h>
#include "file_writer_base.h"
#include "bluefile.hh"

namespace gr {
  namespace sandia_utils {
    class SANDIA_UTILS_API file_writer_bluefile: public file_writer_base
    {
    private:
      bluefile::BlueFile        *d_writer;
      bool                      d_initialized;

      char                      d_stype;
      int                       d_N;


    public:
      /**
       * \brief Return a shared_ptr to a new instance of sandia_utils::file_writer_base.
       *
       * @param data_type -
       * @param file_type -
       * @param itemsize -
       * @param nsamples -
       * @param rate -
       * @param out_dir -
       * @param name_spec -
       * @param logger - logger to utilize
       */
      file_writer_bluefile(std::string data_type, std::string file_type,
                    size_t itemsize, uint64_t nsamples, int rate,
                    std::string out_dir, std::string name_spec, gr::logger_ptr logger);

      /**
       * Deconstructor
       */
      virtual ~file_writer_bluefile();

      /*!
       * Open a new file
       */
      void open(std::string fname);

      /*!
       * Close the current file
       */
      void close();

      /*!
       * Write data
       */
      int write_impl(const void *in, int nitems);
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_WRITER_IMPL_H */
