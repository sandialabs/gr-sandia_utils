/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_SIGMF_METADATA_H
#define INCLUDED_SANDIA_SIGMF_METADATA_H

#include <stdint.h> /* uint64_t */
#include <stdio.h>  /* sscanf, snprintf */
#include <string.h> /* memset */
#include <time.h>   /* gmtime, strftime, timegm */
#include <cmath>    /* modf */

#include "epoch_time.h"

#include <boost/filesystem/path.hpp>
#include <stdexcept>
#include <string>

namespace gr {
namespace sandia_utils {

/*!
 * Helpers shared by the SigMF file reader and writer.
 *
 * A SigMF recording is a pair of files sharing a base name: the samples in
 * <base>.sigmf-data and the JSON metadata in <base>.sigmf-meta.
 */
constexpr const char* SIGMF_DATA_EXT = ".sigmf-data";
constexpr const char* SIGMF_META_EXT = ".sigmf-meta";
constexpr const char* SIGMF_VERSION = "1.0.0";

/*!
 * \brief Strip the extension from a file name
 *
 * Returns the base name shared by the data and metadata files.  Either member
 * of the pair, or the base name itself, may be specified.
 */
inline std::string sigmf_basename(const std::string& filename)
{
    return boost::filesystem::path(filename).replace_extension().string();
}

/*!
 * \brief Number of bytes per sample of a SigMF datatype
 *
 * Returns 0 if the datatype can not be interpreted.
 */
inline size_t sigmf_datatype_size(const std::string& datatype)
{
    if (datatype.empty()) {
        return 0;
    }

    // complex types are prefixed with 'c', real types with 'r'
    size_t ncomponents = (datatype[0] == 'c') ? 2 : 1;
    std::string type = datatype.substr(1);

    // discard the endianness suffix
    type = type.substr(0, type.find('_'));

    // remaining characters are the format character and the number of bits
    size_t nbits = 0;
    try {
        nbits = std::stoul(type.substr(1));
    } catch (...) {
        return 0;
    }

    return ncomponents * nbits / 8;
}

/*!
 * \brief SigMF datatype string for a file sink data type
 *
 * All supported types are little-endian.
 */
inline std::string sigmf_datatype(const std::string& data_type)
{
    if (data_type == "complex") {
        return "cf32_le";
    } else if (data_type == "complex_int") {
        return "ci16_le";
    } else if (data_type == "float") {
        return "rf32_le";
    } else if (data_type == "int") {
        return "ri32_le";
    } else if (data_type == "short") {
        return "ri16_le";
    } else if (data_type == "byte") {
        return "ri8";
    }

    throw std::runtime_error("file_sink:unsupported data type for sigmf file");
}

/*!
 * \brief Convert a SigMF core:datetime string to an epoch time
 *
 * The SigMF specification requires ISO-8601 in UTC, so any trailing timezone
 * designator is ignored.  Returns false if the string can not be parsed.
 */
inline bool sigmf_datetime_to_epoch(const std::string& datetime, epoch_time& value)
{
    int year, month, day, hour, minute;
    double seconds;
    if (sscanf(datetime.c_str(),
               "%d-%d-%dT%d:%d:%lf",
               &year,
               &month,
               &day,
               &hour,
               &minute,
               &seconds) != 6) {
        return false;
    }

    struct tm utc_time;
    memset(&utc_time, 0, sizeof(utc_time));
    utc_time.tm_year = year - 1900;
    utc_time.tm_mon = month - 1;
    utc_time.tm_mday = day;
    utc_time.tm_hour = hour;
    utc_time.tm_min = minute;
    utc_time.tm_sec = 0;

    time_t epoch_second = timegm(&utc_time);
    if (epoch_second == (time_t)-1) {
        return false;
    }

    double frac, sec;
    frac = modf(seconds, &sec);
    value.set((uint64_t)epoch_second + (uint64_t)sec, frac, 1.0);

    return true;
}

/*!
 * \brief Convert an epoch time to a SigMF core:datetime string
 */
inline std::string sigmf_datetime_from_epoch(epoch_time value)
{
    time_t epoch_second = (time_t)value.epoch_sec();
    struct tm* utc_time = gmtime(&epoch_second);

    char datetime[64];
    size_t nchars = strftime(datetime, sizeof(datetime), "%Y-%m-%dT%H:%M:%S", utc_time);

    // fractional seconds are always less than one so no rounding is necessary
    snprintf(datetime + nchars,
             sizeof(datetime) - nchars,
             ".%06dZ",
             (int)(value.epoch_frac() * 1e6));

    return std::string(datetime);
}

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_SIGMF_METADATA_H */
