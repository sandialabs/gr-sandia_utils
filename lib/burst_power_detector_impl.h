/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifndef INCLUDED_SANDIA_UTILS_BURST_POWER_DETECTOR_IMPL_H
#define INCLUDED_SANDIA_UTILS_BURST_POWER_DETECTOR_IMPL_H

#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/tags.h>
#include <sandia_utils/burst_power_detector.h>
#include <volk/volk.h>

using namespace gr::filter;

namespace gr {
namespace sandia_utils {

class burst_power_detector_impl : public burst_power_detector
{
private:
    int d_naverage;
    int d_nguard;
    double d_threshold;
    int d_holdoff;
    int d_block_size;
    uint64_t d_index;
    int d_state;
    bool d_first_block;
    int d_tap_delay;

    // volk buffers
    float* d_power;
    float* d_ratio;
    float* d_log2;
    float* d_log10;

    // convert to 10*log10() using log2()
    float d_log_base;

    // moving-average filter
    kernel::fft_filter_fff* d_filter;

public:
    burst_power_detector_impl(int naverage, int nguard, double threshold, int holdoff);
    ~burst_power_detector_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

private:
    std::vector<float> conv(std::vector<float>& x, std::vector<float>& y);
    void detect(uint64_t index, float* data);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_BURST_POWER_DETECTOR_IMPL_H */
