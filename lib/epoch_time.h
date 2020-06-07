/* -*- c++ -*- */
/*
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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
    }
    epoch_time(uint64_t second, double frac) : d_sec(second), d_frac(frac) {}
    ~epoch_time() {}

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
    }

    epoch_time& operator+=(const int& N)
    {
        d_frac += (double)N * d_T;
        d_sec += uint64_t(d_frac / 1.0);
        d_frac = fmod(d_frac, 1.0);
    }

    // public getters
    uint64_t epoch_sec() { return d_sec; }
    double epoch_frac() { return d_frac; }
    double dtime() { return ((double)d_sec + d_frac); }
};
} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_EPOCH_TIME_H */
