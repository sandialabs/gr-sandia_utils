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


#ifndef INCLUDED_SANDIA_UTILS_FILE_SOURCE_H
#define INCLUDED_SANDIA_UTILS_FILE_SOURCE_H

#include <sandia_utils/api.h>
#include <gnuradio/sync_block.h>
#include <pmt/pmt.h>

namespace gr {
  namespace sandia_utils {

    /*!
     * \brief GNU Radio Source from file
     * \ingroup sandia_utils
     *
     * Generate data from an input file.
     *
     * This block is functionally equivalent to the in-tree file source block with a
     * message port extension to specify a new input file.  If the 'Force New File'
     * option is chosen, the current file being processed will be closed and the new
     * file opened when commanded.
     *
     * Adding file tags will cause the stream tags available based on the file type
     * to be added to the output stream at the first sample of the file.  Similarly, if
     * the beginning tags are populated, the first sample of every file will contain
     * that tag.
     *
     * PDU sink port allows remote control of the file to be played. PDU must contain
     * a dict with the key of fname. The value associated with fname is the file name
     * that will be replayed.
     *
     */
    class SANDIA_UTILS_API file_source : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<file_source> sptr;

      /*!
       * \brief Create a file source.
       *
       * Opens \p filename as a source of items into a flowgraph. The
       * data is expected to be in the format specified by \p type. The
       * \p itemsize of the block determines the conversion from bits
       * to items.
       *
       * If \p repeat is turned on, the file will repeat the file after
       * it's reached the end.
       *
       * \param itemsize	the size of each item in the file, in bytes
       * \param filename	name of the file to source from
       * \param type file type, Example Values = message, raw, raw_header, bluefile
       * \param repeat	repeat file from start
       * \param force_new Force open new file upon command, regardless of current status
       */
      static sptr make(size_t itemsize, const char *filename, const char *type,
        bool repeat = false, bool force_new = false);

        /*!
         * \brief seek file to \p seek_point relative to \p whence
         *
         * \param seek_point	sample offset in file
         * \param whence	one of SEEK_SET, SEEK_CUR, SEEK_END (man fseek)
         * @return bool - 0 or false on success
         */
        virtual bool seek(long seek_point, int whence) = 0;

        /*!
         * \brief Opens a new file.
         *
         * \param filename	name of the file to source from
         * \param repeat	repeat file from start
         */
        virtual void open(const char *filename, bool repeat) = 0;

        /*!
         * \brief Close the file handle.
         */
        virtual void close() = 0;

        /*!
         * \brief Add a stream tag to the first sample of the file if true
         *
         * \param val Value for first stream tag
         */
        virtual void set_begin_tag(pmt::pmt_t val) = 0;

        /*!
         * \brief Set the number of files that will be queued up for processing
         *
         * \param depth Number of files
         */
        virtual void set_file_queue_depth(size_t depth) = 0;

        /*!
         * \brief Add tags to output stream when file is opened.
         *
         * By default, tags are not added to the output stream except for the
         * first file that is opened.
         *
         * \param tag   Add tags on first sample of file open
         */
         virtual void add_file_tags(bool tag) = 0;

         /*!
          * \brief Set the message hop period
          *
          * Set the amount of time between message emissions from a file in
          * milliseconds.
          *
          * \param period_ms  Emission period (ms)
          */
          virtual void set_msg_hop_period(int period_ms) = 0;
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_SOURCE_H */
