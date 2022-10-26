/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_FILE_SINK_H
#define INCLUDED_SANDIA_UTILS_FILE_SINK_H

#include <gnuradio/sandia_utils/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace sandia_utils {
// trigger type
enum trigger_type_t { MANUAL = 0, TRIGGERED = 1 };

/*!
 * \brief File sink
 * \ingroup sandia_utils
 *
 * Augmented in-tree file sink capabilities to support:
 *   - Various file output types (RAW, RAW+HEADER, BLUEFILE, MESSAGE)
 *   - Dynamic file name based on signal parameters:
 *       - Sampling rate
 *       - Frequency
 *       - Start time
 *   - Manual or triggered based file saving
 *   - Rolling output files based on specified file size (samples)
 *   - Alignment of start of file to nearest second boundary
 *
 * This block will save data to files.  File length specified in number
 * of samples, where 0 will result in a single file being generated.  Successive
 * files increment time and file number accordingly.
 *
 * A new folder can be generated at the start of a new collect.  The folder name
 * will have the format:
 *
 * YYYYMMDD/HH_MM_SS
 *
 * where Y = year, M = month(00-12), D = day (00-31), HH = hour (00-23),
 * MM = minute (00-59), and SS = second (00-60)
 *
 * File names can be specified using all conversion specifications supported by
 * strftime.  Additional the following converation specifications can be used:
 *
 * %fcM      Collection center frequency in MHz
 * %fck      Collection center frequency in kHz
 * %fcc      Collection center frequency in Hz
 * %fsM      Collection sample rate in MHz
 * %fsk      Collection sample rate in kHz
 * %fsc      Collection sample rate in Hz
 * %fd       Generated file number.  Modifiers in the standard form (0,N) can
 *            be specified to determine the number of files generated.  For
 *           example, %03fd will wrap after 1000 files (0-999),
 *           prepending zeros to ensure 3 characters per file number.
 *
 */
class SANDIA_UTILS_API file_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<file_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sandia_utils::file_sink.
     *
     * \param data_type Input data type (complex, complex_int, float, int, short, byte)
     * \param itemsize Size of each item in bytes
     * \param file_type File type specifier
     * \param mode Recording mode (Manual or Triggered)
     * \param nsamples  Number of samples per files
     * \param rate Sampling rate (Hz)
     * \param out_dir Base output directory
     * \param name_spec Name specification format string
     * \param debug turn on debug functionality
     */
    static sptr make(std::string data_type,
                     size_t itemsize,
                     std::string file_type,
                     trigger_type_t mode,
                     uint64_t nsamples,
                     int rate,
                     std::string out_dir,
                     std::string name_spec,
                     bool debug = false);

    /*!
     * \brief Set/Get recording state
     *
     * Set the recording state of the system.  If the mode is MANUAL, toggling the
     * state will cause the system to either start recording immediately, or to stop
     * recording immediately.  If the mode is TRIGGERED, toggling the system state
     * will have no effect.
     */
    virtual void set_recording(bool state) = 0;
    virtual bool get_recording() = 0;

    /*!
     * \brief Set/Get second alignment
     *
     * Align the first sample in a file to the sample closest to a second
     * boundary
     */
    virtual void set_second_align(bool align) = 0;
    virtual bool get_second_align() = 0;

    /*!
     * \brief Set/Get recording mode
     *
     * Set or get the current operating mode
     */
    virtual void set_mode(trigger_type_t mode) = 0;
    virtual trigger_type_t get_mode() = 0;

    /*!
     * \brief Set recording mode via ControlPort
     *
     * Set the current operating mode
     */
    void set_mode(int mode) { set_mode((trigger_type_t)mode); }

    /*!
     * \brief Generate new folder on start
     *
     * Generate a new folder for all output files each time the
     * sink is started
     */
    virtual void set_gen_new_folder(bool value) = 0;
    virtual bool get_gen_new_folder() = 0;

    /*!
     * \brief Set/Get center frequency
     *
     */
    virtual void set_freq(int freq) = 0;
    virtual int get_freq() = 0;

    /*!
     * \brief Set/Get sample rate
     *
     */
    virtual void set_rate(int rate) = 0;
    virtual int get_rate() = 0;

    /*!
     * \brief Set/Get number of samples per file
     *
     */
    virtual void set_nsamples(uint64_t nsamples) = 0;
    virtual uint64_t get_nsamples() = 0;

    /*!
     * \brief Set/Get file number rollover
     *
     * The file counter used in the name specifier can be set to rollover
     * at a specified value.  A rollover value less than or equal to 0
     * indicates no rollover value for file number.
     *
     */
    virtual void set_file_num_rollover(int rollover) = 0;
    virtual int get_file_num_rollover() = 0;
};
} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_SINK_H */
