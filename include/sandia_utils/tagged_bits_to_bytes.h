/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_H
#define INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_H

#include <gnuradio/sync_block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Convert tagged bit stream to packed byte stream
 *
 * Block for packing bits into bytes, aligned to a stream tag. This block is generally
 * used after a block like Correlate Access Code Tag, where a preamble is tagged in a
 * stream of bits and you want to pack those bits into bytes starting at that tagged
 * sample.
 *
 * The typical formatting behavior is to pack bits MSB first.  The first
 * bit received is placed into bit 8, the second into bit position 7 and so
 * on.  Little endian formatting allows data to be packed LSB first, i.e the
 * first bit received is put into position 0, the next into position 1 and
 * so on.
 *
 * Example:
 * A bit stream has the third element tagged.
 * __Tag
 * 1010100101
 *
 * The Tag Bits Pack block (in drop stub mode) will output one byte, with the tag.
 * __Tag
 * 0xA5
 *
 * The Tag Bits Pack block in pad right mode will output two bytes, with the tag.
 * ________Tag
 * 0x02,0xA5
 *
 *
 * Time is also added to the byte sample, ralative to the sample rate.
 *
 * Stub Modes:
 *   Drop Stubs: drop the bits that don't fit into a byte before a tag
 *   Pad Right: fill in missing bits to the right
 *   Pad Left: fill in missing bits to the left
 *
 * Input: Bits with tags
 * Output: Bytes
 *
 * \ingroup sandia_utils
 *
 */
class SANDIA_UTILS_API tagged_bits_to_bytes : virtual public gr::block
{
public:
    typedef boost::shared_ptr<tagged_bits_to_bytes> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::tagged_bits_to_bytes.
     *
     * To avoid accidental use of raw pointers, sandia_utils::tagged_bits_to_bytes's
     * constructor is in a private implementation
     * class. sandia_utils::tagged_bits_to_bytes::make is the public interface for
     * creating new instances.
     *
     * \param key Key to start packing
     * \param little_endian Use little-endian packing format(lsb first)
     * \param stub_mode Stub mode
     * \param v_len Output vector length
     * \param sample_rate Sample rate of bit stream

     */
    static sptr make(std::string key = "BURST",
                     bool little_endian = false,
                     int stub_mode = 0,
                     int v_len = 1,
                     double sample_rate = 1);

    /*! \brief Set vector length
     *
     * \param vlen Vector length
     */
    virtual void set_vlen(int vlen) = 0;

    /*! \brief Set little endian mode
     *
     * \param lsb_first Pack bits LSB first
     */
    virtual void set_little_endian_flag(bool lsb_first) = 0;

    /*! \brief Set sample rate
     *
     * \param samp_rate Sample rate of bit stream
     */
    virtual void set_sample_rate(double samp_rate) = 0;
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_TAGGED_BITS_TO_BYTES_H */
