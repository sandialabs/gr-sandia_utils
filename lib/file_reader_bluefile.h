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

#ifndef INCLUDED_SANDIA_UTILS_FILE_READER_BLUEFILE_H
#define INCLUDED_SANDIA_UTILS_FILE_READER_BLUEFILE_H

#include "file_reader_base.h"
#include "epoch_time.h"

#include "bluefile.hh"


namespace gr {
  namespace sandia_utils {
    class SANDIA_UTILS_API file_reader_bluefile : public file_reader_base
    {
    private:
        bluefile::BlueFile        *d_blue_reader;
        std::string               d_type;
        int                       d_bpe;
        int                       d_bps;

    public:
      // typedef boost::shared_ptr<file_reader_base> sptr;

      file_reader_bluefile(size_t itemsize, gr::logger_ptr logger) : file_reader_base(itemsize, logger) {
        std::cout << "generating blue file reader" << std::endl;
      }
      ~file_reader_bluefile() {}

      void open(const char *filename) override {
        if (d_is_open) { this->close(); }

        GR_LOG_DEBUG(d_logger,boost::format("File Reader: Opening file %s") % filename);

        d_blue_reader = new bluefile::BlueFile();

        // open file
        d_file_size = int(d_blue_reader->open(filename,bluefile::BlueFile::READ));
        GR_LOG_DEBUG(d_logger,boost::format("file size: = %d") % d_file_size);
        d_type = d_blue_reader->get_format();
        d_bpe = d_blue_reader->get_bpe();
        d_bps = d_blue_reader->get_bps();
        GR_LOG_DEBUG(d_logger,boost::format("bpe = %d")%d_bpe);
        GR_LOG_DEBUG(d_logger,boost::format("bps = %d")%d_bps);

        // read header information and populate tags - no frequency information right now
        double rate = 1.0 / d_blue_reader->get_xdelta();
        epoch_time file_time(d_blue_reader->get_xstart());

        gr::tag_t tag;
        d_tags.clear();
        // tag.key = FREQ_KEY; tag.value = pmt::from_double(metadata[0]); d_tags.push_back(tag);
        tag.key = RATE_KEY; tag.value = pmt::from_double(rate); d_tags.push_back(tag);
        tag.key = TIME_KEY; tag.value = pmt::make_tuple(pmt::from_uint64(file_time.epoch_sec()),
          pmt::from_double(file_time.epoch_frac()));d_tags.push_back(tag);

        // ensure beginning of file
        d_blue_reader->seek(0,bluefile::BlueFile::SET);

        // all done
        d_is_open = true;
      }

      int read(char *dest, int nitems) override {
        if (d_blue_reader->is_open()) {
          // return d_blue_reader->read(dest,d_bpe,nitems);
          return d_blue_reader->read(dest,d_bps,nitems);
        }

        return 0;
      }

      bool seek(long seek_point, int whence) override {
        if (d_blue_reader->is_open()) {
          d_blue_reader->seek(double(seek_point), bluefile::BlueFile::BlueFileSeekEnum(whence));
        }
        return true;
      }

      void close() override {
        if (d_blue_reader->is_open()) {
          GR_LOG_DEBUG(d_logger,boost::format("File Reader: Closing file %s") % d_filename);
          d_blue_reader->close();

          // set flag
          d_is_open = false;

          // clean up
          delete d_blue_reader;
        }
      }

      bool eof() override {
        if (not d_blue_reader->is_open()) { return true; }

        return (int(d_blue_reader->tell()) == d_file_size);
      }
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_READER_BLUEFILE_H */
