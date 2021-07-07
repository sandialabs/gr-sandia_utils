/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_H
#define INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_H

#include <gnuradio/sync_block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Bit bucket that prints to file any tag received.
 * \ingroup sandia_utils
 *
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
 *
 * This is an extension of the in-tree tag debug block.
 */
class SANDIA_UTILS_API tag_debug_file : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<tag_debug_file> sptr;

    /*!
     * Build a tag debug block
     *
     * \param sizeof_stream_item size of the items in the incoming stream.
     * \param name name to identify which debug sink generated the info.
     * \param key_filter Specify a tag's key value to use as a filter.
     * \param filename to write to
     */
    static sptr make(size_t sizeof_stream_item,
                     const std::string& name,
                     const std::string& key_filter,
                     const std::string& filename);

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
    virtual void set_filewrite(bool w, bool clear = false) = 0;

    /*!
     * \brief Set a new key to filter with.
     */
    virtual void set_key_filter(const std::string& key_filter) = 0;

    /*!
     * \brief Get the current filter key.
     */
    virtual std::string key_filter() const = 0;
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_H */
