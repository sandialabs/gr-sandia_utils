/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_TAG_DEBUG_H
#define INCLUDED_SANDIA_UTILS_TAG_DEBUG_H

#include <gnuradio/sync_block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Copy of blocks::tag_debug with added storage capability
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
 * All tags seen by this block are saved by default. They are
 * accessible by get_tag() and current_tags(). The store_all
 * parameter can control this behavior
 */
class SANDIA_UTILS_API sandia_tag_debug : virtual public sync_block
{
public:
    // gr::blocks::tag_debug::sptr
    typedef boost::shared_ptr<sandia_tag_debug> sptr;

    /*!
     * Build a tag debug block
     *
     * \param sizeof_stream_item size of the items in the incoming stream.
     * \param name name to identify which debug sink generated the info.
     * \param key_filter Specify a tag's key value to use as a filter.
     * \param store_all When true, all tags are stored
     */
    static sptr make(size_t sizeof_stream_item,
                     const std::string& name,
                     const std::string& key_filter = "",
                     bool store_all = true);

    /*!
     * \brief Returns a vector of tag_t items as of the last call to
     * work.
     */
    virtual std::vector<tag_t> current_tags() = 0;

    /**
     * Return a specific tag
     *
     * \param idx Index of saved tag to return
     */
    virtual tag_t get_tag(size_t idx) = 0;

    /**
     * Clears out any stored tags
     */
    virtual void clear_tags(void) = 0;

    /*!
     * \brief Return the total number of tags in the tag queue.
     */
    virtual int num_tags() = 0;

    /*!
     * \brief Set the display of tags to stdout on/off.
     */
    virtual void set_display(bool d) = 0;

    /*!
     * \brief Set a new key to filter with.
     */
    virtual void set_key_filter(const std::string& key_filter) = 0;

    /*!
     * \brief Get the current filter key.
     */
    virtual std::string key_filter() const = 0;
};

} /* namespace sandia_utils */
} /* namespace gr */

#endif /* INCLUDED_SANDIA_UTILS_TAG_DEBUG_H */
