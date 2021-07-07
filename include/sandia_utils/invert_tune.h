/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_INVERT_TUNE_H
#define INCLUDED_SANDIA_UTILS_INVERT_TUNE_H

#include <gnuradio/sync_block.h>
#include <sandia_utils/api.h>

namespace gr {
namespace sandia_utils {

/*!
 * \brief Invert the DSP frequency of a tune request
 *
 * This block will invert the value for key 'dsp_freq' of a tune request
 * if it is present.  This block is necessary due to the differences in
 * handling of CORDIC tunes within UHD between Rx and Tx paths.
 *
 * A tune request is expected to be a dictionary of metadata.The value
 * for the dsp_freq element in that dictionary will be inverted.
 *
 * \ingroup sandia_utils
 *
 */
class SANDIA_UTILS_API invert_tune : virtual public gr::block
{
public:
    typedef std::shared_ptr<invert_tune> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::invert_tune.
     *
     * To avoid accidental use of raw pointers, sandia_utils::invert_tune's
     * constructor is in a private implementation
     * class. sandia_utils::invert_tune::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INVERT_TUNE_H */
