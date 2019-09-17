/* -*- c++ -*- */
/*
 * Copyright 2017 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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
#include "epoch_time.h"

namespace gr {
  namespace sandia_utils {
    class SANDIA_UTILS_API file_reader_raw_header : public file_reader_base
    {
    private:
      // nothing here
    public:
      // typedef boost::shared_ptr<file_reader_base> sptr;

      file_reader_raw_header(size_t itemsize, gr::logger_ptr logger) : file_reader_base(itemsize, logger) {}
      ~file_reader_raw_header() {}

      void open(const char *filename) override {
        file_reader_base::open(filename);

        // read metadata information and populate tags
        if (d_is_open) {
          double metadata[3];
          if (fread(&metadata[0], sizeof(double), 3, d_fp) != 3) {
            throw std::runtime_error("Unable to read metadata from file");
          }

          // set timed
          epoch_time file_time(metadata[2]);

          gr::tag_t tag;
          tag.key = FREQ_KEY; tag.value = pmt::from_double(metadata[0]); d_tags.push_back(tag);
          tag.key = RATE_KEY; tag.value = pmt::from_double(metadata[1]); d_tags.push_back(tag);
          tag.key = TIME_KEY; tag.value = pmt::make_tuple(pmt::from_uint64(file_time.epoch_sec()),
            pmt::from_double(file_time.epoch_frac()));d_tags.push_back(tag);
        }
      }

    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_RAW_HEADER_H */
