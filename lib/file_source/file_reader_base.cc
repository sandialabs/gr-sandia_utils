/* -*- c++ -*- */
/*
 * Copyright 2019 gr-sandia_utils author.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "file_reader_base.h"
// #include "../file_source_impl.h"
#include <sys/stat.h>
#include <fcntl.h>


namespace gr {
  namespace sandia_utils {

    /*
     * The public constructor
     */
    file_reader_base::file_reader_base(size_t itemsize, gr::logger_ptr logger)
      : d_itemsize(itemsize),
        d_logger(logger),
        d_is_open(false)
    {
      d_tags.resize(0);
    }

    /*
     * The public destructor
     */
    file_reader_base::~file_reader_base()
    {
      if (d_is_open) { this->close(); }
    }

    void
    file_reader_base::open(const char *filename) {
      if (d_is_open) { this->close(); }

      GR_LOG_DEBUG(d_logger,boost::format("File Reader: Opening file %s") % filename);

      // clear all tags
      d_tags.clear();

      // we use "open" to use to the O_LARGEFILE flag
      int fd;
      if((fd = ::open(filename, O_RDONLY | OUR_O_LARGEFILE | OUR_O_BINARY)) < 0) {
        perror(filename);
        throw std::runtime_error("can't open file");
      }

      if((d_fp = fdopen (fd, "rb")) == NULL) {
        perror(filename);
        ::close(fd);	// don't leak file descriptor if fdopen fails
        throw std::runtime_error("can't open file");
      }

      //Check to ensure the file will be consumed according to item size
      fseek(d_fp, 0, SEEK_END);
      d_file_size = ftell(d_fp);
      rewind (d_fp);

      d_is_open = true;
    }

    void
    file_reader_base::close() {
      if ((d_is_open) and (d_fp != NULL)) {
        fclose(d_fp);
        d_is_open = false;
        d_fp = NULL;
      }
    }

    /**
     * Seek in the file source
     *
     * @param seek_point - offset position to seek to
     * @param whence - refrence point for seek, see fseek
     * @return bool - 0 or false on success
     */
    bool file_reader_base::seek(long seek_point, int whence)
    {
      return fseek((FILE*)d_fp, seek_point *d_itemsize, whence) == 0;
    }

    /**
     * Read items from file.
     * Number of bytes read from file is based on #d_itemsize
     *
     * @param dest - destination storage for sample
     * @param nitems - number of items to ready
     * @return int - number of items read. 0 on EOF or error
     */
    int file_reader_base::read(char *dest, int nitems)
    {
      if (d_is_open) {
        return fread(dest, d_itemsize,nitems,(FILE *)d_fp);
      }
      else { return 0; }
    }

  } /* namespace sandia_utils */
} /* namespace gr */
