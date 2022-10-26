/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_IMPL_H
#define INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_IMPL_H

#include <gnuradio/sandia_utils/tag_debug_file.h>
#include <gnuradio/thread/thread.h>
#include <stddef.h>

namespace gr {
namespace sandia_utils {

class tag_debug_file_impl : public tag_debug_file
{
private:
    std::string d_name;
    std::string d_filename;
    std::vector<tag_t> d_tags;
    std::vector<tag_t>::iterator d_tags_itr;
    bool d_display;
    bool d_write_file;
    pmt::pmt_t d_filter;
    gr::thread::mutex d_mutex;

public:
    tag_debug_file_impl(size_t sizeof_stream_item,
                        const std::string& name,
                        const std::string& key_filter,
                        const std::string& filename);
    ~tag_debug_file_impl();

    void setup_rpc();

    std::vector<tag_t> current_tags();
    int num_tags();

    void set_display(bool d);
    void set_filewrite(bool w, bool clear);

    void set_key_filter(const std::string& key_filter);
    std::string key_filter() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_TAG_DEBUG_FILE_IMPL_H */
