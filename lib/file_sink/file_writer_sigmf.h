/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_FILE_WRITER_SIGMF_H
#define INCLUDED_SANDIA_UTILS_FILE_WRITER_SIGMF_H

#include "file_writer_base.h"
#include <gnuradio/sandia_utils/api.h>
#include <fstream>

namespace gr {
  namespace sandia_utils {
    /*!
     * \brief Writes a SigMF recording
     *
     * Samples are written to <base>.sigmf-data and the metadata to the
     * companion <base>.sigmf-meta file, where the base name is the file name
     * generated from the name specification with any extension removed.  The metadata file is written when
     * the recording is opened, so the frequency, sample rate and sample time in
     * effect at that point are the ones captured.
     */
    class SANDIA_UTILS_API file_writer_sigmf: public file_writer_base
    {
    private:
      std::ofstream         d_outfile;

      // sigmf datatype of the samples being written
      std::string           d_datatype;

      /*!
       * Write the companion metadata file
       */
      void write_meta(std::string fname);

    public:
      file_writer_sigmf(std::string data_type, std::string file_type,
                    size_t itemsize, uint64_t nsamples, int rate,
                    std::string out_dir, std::string name_spec, gr::logger_ptr logger);
      ~file_writer_sigmf();

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

#endif /* INCLUDED_SANDIA_UTILS_FILE_WRITER_SIGMF_H */
