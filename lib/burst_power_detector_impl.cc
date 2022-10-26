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

#include "burst_power_detector_impl.h"
#include <gnuradio/io_signature.h>
#include <math.h>
#include <pmt/pmt.h>

#define MULT 1
namespace gr {
namespace sandia_utils {

burst_power_detector::sptr
burst_power_detector::make(int naverage, int nguard, double threshold, int holdoff)
{
    return gnuradio::get_initial_sptr(
        new burst_power_detector_impl(naverage, nguard, threshold, holdoff));
}

/*
 * The private constructor
 */
static int ios[] = { sizeof(gr_complex), sizeof(float) };
static std::vector<int> iosig(ios, ios + sizeof(ios) / sizeof(int));
burst_power_detector_impl::burst_power_detector_impl(int naverage,
                                                     int nguard,
                                                     double threshold,
                                                     int holdoff)
    : gr::sync_block("burst_power_detector",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::makev(1, 2, iosig)),
      d_naverage(naverage),
      d_holdoff(holdoff),
      d_nguard(nguard)
{
    // ensure buffers are SIMD aligned
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));

    // set initial state to _searching_
    d_state = 0;

    // ensure we skip first block which will have corrupted data
    d_first_block = true;

    // TODO: error checking

    // threshold specified in dB
    if (threshold < 0) {
        throw std::runtime_error("Invalid threshold value.  Must be greater than 0");
    }
    d_threshold = threshold;

    // initialize moving-average filter
    std::vector<float> x(d_naverage, 1.0 / (float)d_naverage);
    std::vector<float> y(d_nguard + 2);
    y[0] = 1;
    y[d_nguard + 1] = -1;
    std::vector<float> taps = conv(x, y);
    d_tap_delay = int((taps.size() - 1) / 2);

    // generate filter kernel
    d_filter = new kernel::fft_filter_fff(1, taps);

    /* ===================================================================================
     * The function set_taps will return the value of nsamples that can be used externally
     * to check this boundary. Notice that all implementations of the fft_filter GNU Radio
     * blocks (e.g., gr::filter::fft_filter_fff) use this value of nsamples to compute the
     * value to call gr::block::set_output_multiple that ensures the scheduler always
     * passes this block the right number of samples
     * =================================================================================
     */
    d_block_size = d_filter->set_taps(taps);

    // allocate space for volk power operation
    d_power = (float*)volk_malloc(sizeof(float) * d_block_size, volk_get_alignment());
    d_ratio = (float*)volk_malloc(sizeof(float) * d_block_size, volk_get_alignment());


    // store log2(10) for dB conversion
    d_log2 = (float*)volk_malloc(sizeof(float) * d_block_size, volk_get_alignment());
    d_log10 = (float*)volk_malloc(sizeof(float) * d_block_size, volk_get_alignment());
    d_log_base = d_threshold / (log2(10.0));

    // ensure we get a block of data of a specified size
    set_output_multiple(d_block_size);

    // compute number of samples to keep in front of data, including
    // holdoff samples in front of signal detection
    d_holdoff = holdoff + d_tap_delay;

    // We need to keep d_holdoff samples
    // in the buffer to be able to tag a burst at it's start.
    // Set the history to this + 1, so we always have
    // this amount of samples available at the start of
    // our input buffer.
    set_history(d_holdoff + 1);
}

/*
 * Our virtual destructor.
 */
burst_power_detector_impl::~burst_power_detector_impl()
{
    // cleanup
    volk_free(d_power);
    volk_free(d_ratio);
    volk_free(d_log2);
    volk_free(d_log10);
    delete d_filter;
}

int burst_power_detector_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    // we keep d_holdoff samples in front of our data
    const gr_complex* in = (const gr_complex*)input_items[0] + d_holdoff;
    gr_complex* out = (gr_complex*)output_items[0];
    float* pow = (float*)output_items[1];

    for (int i = 0; i < noutput_items; i += d_block_size) {
        d_index = nitems_read(0) + i;

        // compute instantaneous power
        //         volk_32fc_magnitude_32f(d_power,&in[i],d_block_size);
        volk_32fc_magnitude_squared_32f(d_power, &in[i], d_block_size);

        // add some small value to ensure we don't take the log of zero
        float eps = 1e-12;
        for (int j = 0; j < d_block_size; ++j) {
            d_power[j] += eps;
        }

        // convert to dB
        volk_32f_log2_32f(d_log2, d_power, d_block_size);
        volk_32f_s32f_multiply_32f(d_log10, d_log2, d_log_base, d_block_size);

        // average
        d_filter->filter(d_block_size, d_log10, d_ratio);
        if (d_first_block) {
            d_first_block = false;
            continue;
        }

        // copy to output
        memcpy(&pow[i], d_ratio, sizeof(float) * d_block_size);

        // detect
        detect(d_index, d_ratio);
    }

    // copy data to output
    memcpy(out, in - d_holdoff, sizeof(gr_complex) * noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

// brute-force convolution
std::vector<float> burst_power_detector_impl::conv(std::vector<float>& x,
                                                   std::vector<float>& y)
{
    size_t N = x.size();
    size_t M = y.size();
    size_t P = N + M - 1;

    // zero-pad inputs
    std::vector<float> a(P, 0);
    std::vector<float> b(P, 0);
    std::vector<float> c(P, 0);

    // copy data in
    memcpy(&a[0], &x[0], sizeof(float) * N);
    memcpy(&b[0], &y[0], sizeof(float) * M);

    for (size_t ii = 0; ii < P; ++ii) {
        for (size_t jj = 0; jj <= ii; ++jj) {
            c[ii] += (a[jj] * b[ii - jj]);
        }
    }

    return c;
}

void burst_power_detector_impl::detect(uint64_t dindex, float* data)
{
    pmt::pmt_t key, value;


    //float d_val;
    //int ii = 0;
    for (int ii = 0; ii < d_block_size; ++ii) {
        // brute force for now - need to find faster implementation
        if (d_state == 0) {
            if (data[ii] > d_threshold) {
                // add tag
                key = pmt::string_to_symbol("sob");
                value = pmt::from_double(data[ii]);
                add_item_tag(0, d_index + ii + d_tap_delay, key, value);

                // update state
                d_state = 1;
            }
        } else {
            if (data[ii] < -d_threshold) {
                // add tag
                key = pmt::string_to_symbol("eob");
                value = pmt::from_double(data[ii]);
                add_item_tag(0, d_index + ii + 2 * d_tap_delay + d_holdoff, key, value);

                // update state
                d_state = 0;
            }
        }
    }
} /* end detect() */


} /* namespace sandia_utils */
} /* namespace gr */
