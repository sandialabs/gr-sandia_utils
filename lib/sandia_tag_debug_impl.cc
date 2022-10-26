/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sandia_tag_debug_impl.h"
#include <gnuradio/io_signature.h>
#include <iomanip>
#include <iostream>

namespace gr {
namespace sandia_utils {

sandia_tag_debug::sptr sandia_tag_debug::make(size_t sizeof_stream_item,
                                              const std::string& name,
                                              const std::string& key_filter,
                                              bool store_all)
{
    return gnuradio::get_initial_sptr(
        new sandia_tag_debug_impl(sizeof_stream_item, name, key_filter, store_all));
}

sandia_tag_debug_impl::sandia_tag_debug_impl(size_t sizeof_stream_item,
                                             const std::string& name,
                                             const std::string& key_filter,
                                             bool store_all)
    : sync_block("tag_debug",
                 io_signature::make(1, -1, sizeof_stream_item),
                 io_signature::make(0, 0, 0)),
      d_name(name),
      d_display(true),
      d_storeall(store_all)
{
    set_key_filter(key_filter);
}

sandia_tag_debug_impl::~sandia_tag_debug_impl() {}

std::vector<tag_t> sandia_tag_debug_impl::current_tags()
{
    gr::thread::scoped_lock l(d_mutex);
    return d_tags;
}

int sandia_tag_debug_impl::num_tags() { return d_tags.size(); }

tag_t sandia_tag_debug_impl::get_tag(size_t idx)
{
    tag_t ans;

    gr::thread::scoped_lock l(d_mutex);
    if (idx < d_tags.size()) {
        ans = d_tags[idx];
    }

    return ans;
}

/**
 * Clears out any stored tags
 */
void sandia_tag_debug_impl::clear_tags(void)
{
    d_tags.clear();

    return;
}

void sandia_tag_debug_impl::set_display(bool d) { d_display = d; }

void sandia_tag_debug_impl::set_key_filter(const std::string& key_filter)
{
    if (key_filter.empty())
        d_filter = pmt::PMT_NIL;
    else
        d_filter = pmt::intern(key_filter);
}

std::string sandia_tag_debug_impl::key_filter() const
{
    return pmt::symbol_to_string(d_filter);
}

int sandia_tag_debug_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_mutex);
    bool toprint = false;
    std::vector<tag_t> temp_tags;
    std::vector<tag_t>::iterator tags_itr;
    uint64_t abs_N, end_N;
    std::stringstream sout;


    if (d_display) {
        sout << std::endl
             << "----------------------------------------------------------------------";
        sout << std::endl << "Tag Debug: " << d_name << std::endl;
    }

    if (!d_storeall) {
        d_tags.clear();
    }

    for (size_t i = 0; i < input_items.size(); i++) {
        abs_N = nitems_read(i);
        end_N = abs_N + (uint64_t)(noutput_items);

        // clear previous tags to ensure only the most recent from this input
        temp_tags.clear();
        if (pmt::is_null(d_filter)) {
            get_tags_in_range(temp_tags, i, abs_N, end_N);
        } else {
            get_tags_in_range(temp_tags, i, abs_N, end_N, d_filter);
        }

        // copy over the tags to our local vector
        for (size_t i = 0; i < temp_tags.size(); i++) {
            d_tags.push_back(temp_tags[i]);
        }

        if (!temp_tags.empty()) {
            toprint = true;
        }

        if (toprint) {
            sout << "Input Stream: " << std::setw(2) << std::setfill('0') << i
                 << std::setfill(' ') << std::endl;
            for (tags_itr = temp_tags.begin(); tags_itr != temp_tags.end(); tags_itr++) {
                sout << std::setw(10) << "Offset: " << tags_itr->offset << std::setw(10)
                     << "Source: "
                     << (pmt::is_symbol(tags_itr->srcid)
                             ? pmt::symbol_to_string(tags_itr->srcid)
                             : "n/a")
                     << std::setw(10) << "Key: " << pmt::symbol_to_string(tags_itr->key)
                     << std::setw(10) << "Value: ";
                sout << tags_itr->value << std::endl;
            }
        } // end if( d_display
    }     // end for( input_items

    if (d_display && toprint) {
        sout << "----------------------------------------------------------------------";
        sout << std::endl;

        std::cout << sout.str();
    }

    return noutput_items;
} // end work

void sandia_tag_debug_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<sandia_tag_debug, int>(alias(),
                                                         "num. tags",
                                                         &sandia_tag_debug::num_tags,
                                                         pmt::from_long(0),
                                                         pmt::from_long(10000),
                                                         pmt::from_long(0),
                                                         "",
                                                         "Number of Tags",
                                                         RPC_PRIVLVL_MIN,
                                                         DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
}

} /* namespace sandia_utils */
} /* namespace gr */
