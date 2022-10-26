/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_FILE_WRITER_RAW_H
#define INCLUDED_SANDIA_UTILS_FILE_WRITER_RAW_H

#include "file_writer_base.h"
#include <gnuradio/sandia_utils/api.h>
#include <fstream>

namespace gr {
  namespace sandia_utils {
    class SANDIA_UTILS_API file_writer_raw: public file_writer_base
    {
    private:
      std::ofstream         d_outfile;

    public:
      file_writer_raw(std::string data_type, std::string file_type,
                    size_t itemsize, uint64_t nsamples, int rate,
                    std::string out_dir, std::string name_spec, gr::logger_ptr logger);
      ~file_writer_raw();

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
