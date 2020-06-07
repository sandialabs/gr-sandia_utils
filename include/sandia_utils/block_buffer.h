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

#ifndef INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_H
#define INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_H

#include <gnuradio/block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Guarantee blocks of continuous samples, free of overflows or rate changes.
 * \ingroup sandia_utils
 *
 * This block internally buffers the specified number of samples. It
 * watches for rx_time tags, which indicate overflow, sample rate change,
 * or frequency changes from the source.
 *
 * A BLOCK tag is output with the first sample of each block. The value of
 * the tag is the number of samples which have been skipped since the last
 * block (not counting any skips from source overflows).
 *
 * An example use case would be to place this block immediately after a
 * source, proceeded by a throttle. Then, the source can be set to a high
 * sample rate, while the processing is done at a lower, configurable rate.
 * Only connect one block to the output of this block as odd behavior has
 * been observed with multiple connected blocks due to the scheduler.
 *
 */
class SANDIA_UTILS_API block_buffer : virtual public gr::block
{
public:
    typedef boost::shared_ptr<block_buffer> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::block_buffer.
     *
     * To avoid accidental use of raw pointers, sandia_utils::block_buffer's
     * constructor is in a private implementation
     * class. sandia_utils::block_buffer::make is the public interface for
     * creating new instances.
     *
     * \param itemsize    Size of items in bytes
     * \param nsamples    Number of samples per block
     * \param samp_rate   Sample rate
     * \param pass_data   Pass data through or block
     */
    static sptr
    make(size_t itemsize, uint64_t nsamples, float samp_rate, bool pass_data = true);

    /*! \brief Set number of samples in the buffer
     *
     * \param nsamples Number of samples
     */
    virtual void set_nsamples(uint64_t nsamples) = 0;

    /*! \brief Set whether to flow data or drop on the floor
     *
     * \param pass_data Flag to pass data_type
     */
    virtual void set_pass_data(bool pass_data) = 0;

    /*! \brief Get current pass_data state
     *
     * \return State of passing data
     */
    virtual bool get_pass_data() = 0;
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_BLOCK_BUFFER_H */
