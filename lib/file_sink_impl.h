/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of Sandia, LLC
 * (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
 * retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SANDIA_UTILS_FILE_SINK_IMPL_H
#define INCLUDED_SANDIA_UTILS_FILE_SINK_IMPL_H

#include "epoch_time.h"
#include "file_sink/file_writer_base.h"
#include <gnuradio/sandia_utils/constants.h>
#include <gnuradio/sandia_utils/file_sink.h>
#include <fstream>

namespace gr {
namespace sandia_utils {

class file_sink_impl : public file_sink
{
private:
    std::string d_type;
    size_t d_itemsize;
    trigger_type_t d_mode;
    int d_freq;
    uint64_t d_nsamples;
    std::string d_out_dir;
    std::string d_name_spec;

    // sampling period
    double d_T;

    // base class
    file_writer_base::sptr d_file_writer;

    // state of recording
    bool d_recording;
    bool d_check_start;
    bool d_issue_start;

    // mode state
    // 0 - waiting for start of burst
    // 1 - waiting for end of burst
    int d_burst_state;

    // number of samples to discard
    int d_ndiscard;

    // align to secondary boundary
    bool d_align;

    // sample timestamp (epoch integer and fractional second)
    epoch_time d_samp_time;

    // protection
    boost::recursive_mutex d_mutex;

    // message stream file object
    std::ofstream d_msg_file;

    bool d_debug;

public:
    /**
     * Constructor
     *
     * @param type - Input data type (complex, float, int, short, byte)
     * @param itemsize - Size of each item in bytes
     * @param file_type - File type specifier
     * @param mode - Recording mode (Manual or Triggered)
     * @param nsamples - Number of samples per files
     * @param rate - Sampling rate (Hz)
     * @param out_dir - Base output directory
     * @param name_spec - Name specification format string
     * @param debug - turn on debug functionality
     */
    file_sink_impl(std::string type,
                   size_t itemsize,
                   std::string file_type,
                   trigger_type_t mode,
                   uint64_t nsamples,
                   int rate,
                   std::string out_dir,
                   std::string name_spec,
                   bool debug = false);

    /**
     * Deconstructor
     */
    ~file_sink_impl();

    void handle_msg(pmt::pmt_t msg);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    /**
     * Lifecycle start method
     */
    virtual bool start();

    /**
     * Lifecycle stop method
     *
     */
    bool stop();

    // publish updates
    /**
     * Send PDU update after file close
     *
     * @param fname - filename notifying on
     * @param file_time - time of first sample
     * @param freq - center frequency
     * @param rate - sample rate
     * @param samples - number of samples in the file
     */
    void send_update(std::string fname,
                     epoch_time file_time,
                     double freq,
                     double rate,
                     uint64_t samples);


    // setup rpc
    void setup_rpc();

    // set/get recording state
    void set_recording(bool state);
    bool get_recording();

    // set second  alignment
    void set_second_align(bool align) { d_align = align; }
    bool get_second_align() { return d_align; }

    // set/get mode
    void set_mode(trigger_type_t mode);
    trigger_type_t get_mode();

    // set/get number of samples per file
    void set_nsamples(uint64_t nsamples)
    {
        if (d_type != "message") {
            d_file_writer->set_nsamples(nsamples);
        }
    }
    uint64_t get_nsamples()
    {
        if (d_type == "message") {
            return 0;
        } else {
            return d_file_writer->get_nsamples();
        }
    }

    // set/get file number rollover value
    void set_file_num_rollover(int rollover)
    {
        if (d_type != "message") {
            d_file_writer->set_file_num_rollover(rollover);
        }
    }
    int get_file_num_rollover()
    {
        if (d_type == "message") {
            return 0;
        } else {
            return d_file_writer->get_file_num_rollover();
        }
    }

    // set/get new folder
    void set_gen_new_folder(bool mode);
    bool get_gen_new_folder()
    {
        if (d_type != "messsage") {
            return false;
        } else {
            return d_file_writer->get_gen_new_folder();
        }
    }

    // set center freq
    void set_freq(int freq)
    {
        if (d_type != "message") {
            d_file_writer->set_freq(freq);
        }
    }
    int get_freq()
    {
        if (d_type == "message") {
            return 0;
        } else {
            return d_file_writer->get_freq();
        }
    }

    // set sample rate
    void set_rate(int rate)
    {
        if (d_type != "message") {
            d_file_writer->set_rate(rate);
        }
    }
    int get_rate()
    {
        if (d_type == "message") {
            return 0;
        } else {
            return d_file_writer->get_rate();
        }
    }

private:
    void do_set_recording(bool state);
    void do_set_mode(trigger_type_t mode);
    int do_handle_tags(std::vector<tag_t>& tags,
                       uint64_t starting_offset,
                       bool& do_stop,
                       int noutput_items);

}; // end class file_sink_impl

} // namespace sandia_utils
} // namespace gr

#endif /* INCLUDED_SANDIA_UTILS_FILE_SINK_IMPL_H */
