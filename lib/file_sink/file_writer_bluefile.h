/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
