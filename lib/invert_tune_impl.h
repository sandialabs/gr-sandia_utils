/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_INVERT_TUNE_IMPL_H
#define INCLUDED_SANDIA_UTILS_INVERT_TUNE_IMPL_H

#include <gnuradio/sandia_utils/invert_tune.h>

namespace gr {
namespace sandia_utils {

class invert_tune_impl : public invert_tune
{
private:
    uint64_t d_n_error;
    uint64_t d_n_exception;
    uint64_t d_n_pdu;
    uint64_t d_n_ops;


public:
    invert_tune_impl();
    ~invert_tune_impl();
    bool stop();

    void handle_msg(pmt::pmt_t);
};

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_INVERT_TUNE_IMPL_H */
