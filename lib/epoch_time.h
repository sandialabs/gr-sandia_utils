/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_EPOCH_TIME_H
#define INCLUDED_SANDIA_EPOCH_TIME_H

#include <sys/time.h> /* struct timeval, gettimeofday */
#include <cmath>      /* modf */
#include <iostream>

namespace gr {
namespace sandia_utils {

class epoch_time
{
private:
    uint64_t d_sec;
    double d_frac;
    double d_T;

public:
    epoch_time() {}
    epoch_time(double seconds)
    {
        double frac, sec;
        frac = modf(seconds, &sec);
        d_sec = static_cast<uint64_t>(sec);
        d_frac = frac;
        d_T = 1.0;
    }
    epoch_time(uint64_t second, double frac) : d_sec(second), d_frac(frac), d_T(1.0) {}
    ~epoch_time() {}

    // copy constructor
    epoch_time(const epoch_time& t)
    {
        d_sec = t.d_sec;
        d_frac = t.d_frac;
        d_T = t.d_T;
    }


    // set operator
    void set(uint64_t second, double frac, double T)
    {
        d_sec = second;
        d_frac = frac;
        d_T = T;
    }

    // set period only
    void set_T(double T)
    {
        d_T = T;
        // std::cout << "setting T to " << d_T << std::endl;
    }

    // overload add operator
    epoch_time& operator+=(const double& frac)
    {
        d_frac += frac;
        d_sec += uint64_t(d_frac / 1.0);
        d_frac = fmod(d_frac, 1.0);

        return *this;
    }

    epoch_time& operator+=(const int& N)
    {
        d_frac += (double)N * d_T;
        d_sec += uint64_t(d_frac / 1.0);
        d_frac = fmod(d_frac, 1.0);

        return *this;
    }

    // public getters
    uint64_t epoch_sec() { return d_sec; }
    double epoch_frac() { return d_frac; }
    double dtime() { return ((double)d_sec + d_frac); }
};
} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_EPOCH_TIME_H */
