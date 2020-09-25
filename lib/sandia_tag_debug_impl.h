/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_SANDIA_TAG_DEBUG_IMPL_H
#define INCLUDED_SANDIA_UTILS_SANDIA_TAG_DEBUG_IMPL_H

#include <gnuradio/thread/thread.h>
#include <sandia_utils/sandia_tag_debug.h>
#include <stddef.h>

namespace gr {
namespace sandia_utils {

class sandia_tag_debug_impl : public sandia_tag_debug
{
private:
    std::string d_name;
    std::vector<tag_t> d_tags;
    bool d_display;
    pmt::pmt_t d_filter;
    gr::thread::mutex d_mutex;
    bool d_storeall;

public:
    sandia_tag_debug_impl(size_t sizeof_stream_item,
                          const std::string& name,
                          const std::string& key_filter = "",
                          bool store_all = true);
    ~sandia_tag_debug_impl();

    void setup_rpc();

    std::vector<tag_t> current_tags();
    int num_tags();
    /**
     * Return a specific tag
     *
     * \param idx Index of saved tag to return
     */
    virtual tag_t get_tag(size_t idx);

    /**
     * Clears out any stored tags
     */
    virtual void clear_tags(void);

    void set_display(bool d);

    void set_key_filter(const std::string& key_filter);
    std::string key_filter() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace sandia_utils */
} /* namespace gr */

#endif /* INCLUDED_SANDIA_UTILS_SANDIA_TAG_DEBUG_IMPL_H */
