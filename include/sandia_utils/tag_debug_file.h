/* -*- c++ -*- */
/*
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
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


#ifndef INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_H
#define INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_H

#include <sandia_utils/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace sandia_utils {

    /*!
     * \brief Bit bucket that prints out any tag received.
     * \ingroup measurement_tools_blk
     * \ingroup stream_tag_tools_blk
     * \ingroup debug_tools_blk
     *
     * \details
     * This block collects all tags sent to it on all input ports and
     * displays them to stdout in a formatted way. The \p name
     * parameter is used to identify which debug sink generated the
     * tag, so when connecting a block to this debug sink, an
     * appropriate name is something that identifies the input block.
     *
     * This block otherwise acts as a NULL sink in that items from the
     * input stream are ignored. It is designed to be able to attach
     * to any block and watch all tags streaming out of that block for
     * debugging purposes.
     *
     * Specifying a key will allow this block to filter out all other
     * tags and only display tags that match the given key. This can
     * help clean up the output and allow you to focus in on a
     * particular tag of interest.
     *
     * The tags from the last call to this work function are stored
     * and can be retrieved using the function 'current_tags'.
     */
    class SANDIA_UTILS_API tag_debug_file : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<tag_debug_file> sptr;

      /*!
       * Build a tag debug block
       *
       * \param sizeof_stream_item size of the items in the incoming stream.
       * \param name name to identify which debug sink generated the info.
       * \param key_filter Specify a tag's key value to use as a filter.
       * \param filename to log to
       */
      static sptr make(size_t sizeof_stream_item, const std::string &name, const std::string &key_filter, const std::string &filename);

      /*!
       * \brief Returns a vector of tag_t items as of the last call to
       * work.
       */
      virtual std::vector<tag_t> current_tags() = 0;

      /*!
       * \brief Return the total number of tags in the tag queue.
       */
      virtual int num_tags() = 0;

      /*!
       * \brief Set the display of tags to stdout on/off.
       */
      virtual void set_display(bool d) = 0;

      /*!
       * \brief Set the writing of tags to file.
       */
      virtual void set_filewrite(bool w, bool clear=false) = 0;

      /*!
       * \brief Set a new key to filter with.
       */
      virtual void set_key_filter(const std::string &key_filter) = 0;

      /*!
       * \brief Get the current filter key.
       */
      virtual std::string key_filter() const = 0;
    };

  } // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_H */
