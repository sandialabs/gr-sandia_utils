/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "file_reader_base.h"
// #include "../file_source_impl.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/format.hpp>

namespace gr {
  namespace sandia_utils {

    /*
     * The public constructor
     */
  file_reader_base::file_reader_base(size_t itemsize, gr::logger_ptr logger)
      : d_itemsize(itemsize), d_logger(logger), d_is_open(false), d_fp(NULL)
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

      d_logger->debug("File Reader: Opening file {}", filename);

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
        // no need to seek if not defined
        if (not d_fp)
            return true;

        return fseek((FILE*)d_fp, seek_point * d_itemsize, whence) == 0;
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
