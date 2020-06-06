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


#ifndef INCLUDED_SANDIA_UTILS_FILE_SINK_H
#define INCLUDED_SANDIA_UTILS_FILE_SINK_H

#include <sandia_utils/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace sandia_utils {
    // trigger type
    enum trigger_type_t {
      MANUAL = 0,
      TRIGGERED = 1
    };

    /*!
     * \brief File Sink with extra type support and extra file ops support
     * \ingroup sandia_utils
     *
     *
     * This block will save data to files.  File length specified in number
     * of samples, where 0 will result in a single file being generated.  Successive
     * files increment time and file number accordingly.

     * A new folder can be generated at the start of a new collect.  The folder name
     * will have the format:
     *
     * YYYYMMDD/HH_MM_SS
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
     *      be specified to determine the number of files generated.  For
     *      example, %03fd will wrap after 1000 files, prepending zeros to ensure
     *      3 characters per file number.
     *
     *
     */
    class SANDIA_UTILS_API file_sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<file_sink> sptr;

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
       */
      static sptr make(std::string data_type, size_t itemsize, std::string file_type,
                       trigger_type_t mode, uint64_t nsamples, int rate,
                       std::string out_dir, std::string name_spec);

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
