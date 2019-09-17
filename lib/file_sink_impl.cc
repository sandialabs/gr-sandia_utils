/* -*- c++ -*- */
/*
 * Copyright 2017 National Technology & Engineering Solutions of Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains certain rights in this software.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "file_sink_impl.h"

namespace gr {
  namespace sandia_utils {

    file_sink::sptr
    file_sink::make(std::string type, size_t itemsize, std::string file_type,
                    trigger_type_t mode, uint64_t nsamples, int rate,
                    std::string out_dir, std::string name_spec)
    {
      return gnuradio::get_initial_sptr
      (new file_sink_impl(type, itemsize, file_type, mode, nsamples, rate, out_dir, name_spec));
    }

    /*
     * The private constructor
     */
    file_sink_impl::file_sink_impl(std::string data_type, size_t itemsize,
                                   std::string file_type, trigger_type_t mode,
                                   uint64_t nsamples, int rate,
                                   std::string out_dir, std::string name_spec)
      : gr::sync_block("file_sink",
                       gr::io_signature::make(1, 1, itemsize),
                       gr::io_signature::make(0, 0, 0)),
        d_itemsize(itemsize),
        d_mode(mode),
        d_nsamples(nsamples),
        d_out_dir(out_dir),
        d_name_spec(name_spec)
    {
      // initialize writer
      d_file_writer = file_writer_base::make(data_type, file_type, itemsize,
                                             nsamples, rate, out_dir, name_spec,
                                             d_logger);

      // register update callback
      d_file_writer->register_callback(boost::bind(&file_sink_impl::send_update,this,_1));

      // compute sampling period - needed to determine how many samples to
      // discard when starting recording
      d_T = 1.0/d_file_writer->get_rate();

      // set initial local values
      d_recording = false;
      d_recording_changed = false;
      d_issue_start = false;
      d_ndiscard = 0;
      d_burst_state = 0;

      // align on second boundary by default
      d_align = true;

      // set time to system time by default
      // initialize time of next incoming sample to be current system time
      // if tag is included, time will be updated
      // any other time a tag is included, if the system is currently
      // recording, the state will be reset
      // the fractional portion will be set to zero so no samples
      // are discarded
      struct timeval tp;
      gettimeofday(&tp, NULL);
      d_samp_time.set((uint64_t)tp.tv_sec,0.0,1.0/(double)d_file_writer->get_rate());

      // setup output message portion
      message_port_register_out(FILE_MSG_KEY);
    }

    /*
     * Our virtual destructor.
     */
    file_sink_impl::~file_sink_impl()
    {
      /* NOOP */
    }

    /*
     * Setup RPC variables
     */
    void
    file_sink_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<file_sink, bool>(
            alias(), "recording", &file_sink::get_recording,
            pmt::mp(false), pmt::mp(true), pmt::mp(false),
            "Logical", "Recording?", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_set<file_sink, bool>(
            alias(), "recording", &file_sink::set_recording,
            pmt::mp(false), pmt::mp(true), pmt::mp(false),
            "Logical", "Recording?", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<file_sink, trigger_type_t>(
            alias(), "mode", &file_sink::get_mode,
            pmt::mp(0), pmt::mp(0), pmt::mp(0),
            "Integer", "Mode", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_set<file_sink, int>(
            alias(), "mode", &file_sink::set_mode,
            pmt::mp(0), pmt::mp(0), pmt::mp(0),
            "Integer", "Mode", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<file_sink, bool>(
            alias(), "new_folder", &file_sink::get_gen_new_folder,
            pmt::mp(false), pmt::mp(true), pmt::mp(false),
            "Logical", "Gen New Folder?", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_set<file_sink, bool>(
            alias(), "new_folder", &file_sink::set_gen_new_folder,
            pmt::mp(false), pmt::mp(true), pmt::mp(false),
            "Logical", "Gen New Folder?", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_get<file_sink, int>(
            alias(), "freq", &file_sink::get_freq,
            pmt::mp(0), pmt::mp(6000000000), pmt::mp(69000000),
            "Hz", "Frequency", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));

      d_rpc_vars.push_back(
        rpcbasic_sptr(new rpcbasic_register_set<file_sink, int>(
            alias(), "freq", &file_sink::set_freq,
            pmt::mp(0), pmt::mp(6000000000), pmt::mp(69000000),
            "Hz", "Frequency", RPC_PRIVLVL_MIN,
            DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
    }

    int
    file_sink_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      boost::recursive_mutex::scoped_lock lock(d_mutex);

      const void *in = (const void*) input_items[0];

      // number of items processed
      int nprocessed = noutput_items;
      int ntoconsume = noutput_items;

      // get all tags
      uint64_t start = nitems_read(0);
      uint64_t end = start + noutput_items;
      std::vector<tag_t> tags;
      get_tags_in_range(tags,0,start,end);
      bool do_stop = false;   // reset after writing data
      if (tags.size()) {
        // how many items can we actually consume?
        ntoconsume = do_handle_tags(tags,start,do_stop,noutput_items);
      }

      // manual recording mode
      if (d_mode == MANUAL)
      {
        // check if start recording command needs to be issued
        if (d_recording) {
          if (d_recording_changed)
          {
            // compute number of samples to discard
            if ((not d_align) or ( d_samp_time.epoch_frac() - d_T <= 0.0))
            {
              // as close as we can get to the second boundary
              d_ndiscard = 0;
            }
            else
            {
              // TODO: This appears to be one sample off in some cases...figure out why!
              d_ndiscard = (int)std::ceil((double)d_file_writer->get_rate()*(1.0 - d_samp_time.epoch_frac()));
              GR_LOG_DEBUG(d_logger,boost::format("epoch_sec = %ld, epoch_frac = %0.6e, ndiscard = %d")
                    % d_samp_time.epoch_sec() % d_samp_time.epoch_frac() % d_ndiscard);
            }

            d_recording_changed = false;
            d_issue_start = true;
          }

          if (d_ndiscard)
          {
            // throw away as many samples as possible
            nprocessed = (int)std::min(d_ndiscard, ntoconsume);
            d_ndiscard -= nprocessed;
            // GR_LOG_DEBUG(d_logger, boost::format("discarding %d samples") % nprocessed);
          }
          else
          {
            if (d_issue_start) {
              // start file writer if it has been closed
              GR_LOG_DEBUG(d_logger,boost::format("starting writer: sec = %ld, frac = %0.6e")
                     % d_samp_time.epoch_sec() % d_samp_time.epoch_frac());
              d_file_writer->start(d_samp_time);
              d_issue_start = false;
            }

            // write data
            d_file_writer->write(in,ntoconsume);
            nprocessed = ntoconsume;

            // signaled to stop - start will be issued on next tag
            if (do_stop) { d_file_writer->stop(); do_stop = false; }
          }
        } /* end if d_recording */
      }
      else
      {
        // search for burst tags
        std::vector<tag_t> burst_tags;
        if (d_burst_state == 0)
        {
          get_tags_in_range(burst_tags,0,start,end,BURST_START_KEY);
          if (burst_tags.size())
          {
            // move to start of burst and increment state
            nprocessed = burst_tags[0].offset - start;
            d_burst_state = 1;

            // signal to issue start
            d_issue_start = true;
          }
        }
        else
        {
          // issue start if necessary
          if (d_issue_start)
          {
            GR_LOG_DEBUG(d_logger, boost::format("starting burst writer: sec = %ld, frac = %0.6e\n")
                     % d_samp_time.epoch_sec() % d_samp_time.epoch_frac());
            d_file_writer->start(d_samp_time);
            d_issue_start = false;
          }

          get_tags_in_range(burst_tags,0,start,end,BURST_STOP_KEY);
          if (burst_tags.size())
          {
            GR_LOG_DEBUG(d_logger,"Burst stop tag received.  Stopping burst writer...\n");

            // process all samples up to and including tag
            nprocessed = burst_tags[0].offset - start + 1;

            // write data up to end of burst
            d_file_writer->write(in,nprocessed);
            this->stop();
          }
          else
          {
            // consume all available data
            nprocessed = noutput_items;

            // write data
            d_file_writer->write(in,nprocessed);
          }
        }
      }/* end if d_mode */

      // Update sample time
      d_samp_time += nprocessed;
      // std::cout << boost::format("NProcessed: %d, Updating time: (%ld, %0.6f)") % nprocessed % (d_samp_time.epoch_sec()) % (d_samp_time.epoch_frac()) << std::endl;

      // Tell runtime system how many output items we produced.
      return nprocessed;
    }

    int
    file_sink_impl::do_handle_tags(std::vector<tag_t>& tags,
      uint64_t starting_offset, bool& do_stop, int noutput_items)
    {
      int ntoconsume = noutput_items;

      // process all tags, and use the last offset observed to determine
      // how many samples to consume
      uint64_t change_offset = starting_offset + noutput_items;
      bool config_changed = false;

      GR_LOG_DEBUG(d_logger,boost::format("Starting tag offset: %ld") % (starting_offset));
      for (size_t tag_num = 0; tag_num < tags.size(); ++tag_num) {
        GR_LOG_DEBUG(d_logger,boost::format("File Sink Tag %ld: key %s, offset %ld")
          % tag_num % tags[tag_num].key % tags[tag_num].offset);

        // ensure tags that don't affect stream are processed first
        if (pmt::equal(tags[tag_num].key, RATE_KEY))
        {
          d_file_writer->set_rate((int)pmt::to_double(tags[tag_num].value));

          // update delta
          d_T = 1.0/(double)d_file_writer->get_rate();
          d_samp_time.set_T(d_T);
          GR_LOG_DEBUG(d_logger,boost::format("Sample rate set to %d Hz") % d_file_writer->get_rate());

          config_changed = true;
          change_offset = tags[tag_num].offset;
        }
        else if (pmt::equal(tags[tag_num].key, FREQ_KEY))
        {
          d_file_writer->set_freq((int)pmt::to_double(tags[tag_num].value));
          GR_LOG_DEBUG(d_logger,boost::format("Frequency set to %d Hz") % d_file_writer->get_freq());

          config_changed = true;
          change_offset = tags[tag_num].offset;
        }
        else if (pmt::equal(tags[tag_num].key, TIME_KEY))
        {
          pmt::pmt_t time_tuple = tags[tag_num].value;
          if (pmt::is_tuple(time_tuple))
          {
            d_samp_time.set(pmt::to_uint64(pmt::tuple_ref(time_tuple,0)),
                            pmt::to_double(pmt::tuple_ref(time_tuple,1)),
                            1.0/(double)d_file_writer->get_rate());
            GR_LOG_DEBUG(d_logger,boost::format("Updating time: (%ld, %0.6f)") % (d_samp_time.epoch_sec()) % (d_samp_time.epoch_frac()));

            config_changed = true;
            change_offset = tags[tag_num].offset;
          }
        }
        else { /* NOOP */}
      } /* end for tags */

      // The command to start the writer is only issue when the observed tags
      // indicate a configuration change, and the last observed offset for a
      // configuration change is the same as the starting offset
      GR_LOG_DEBUG(d_logger,boost::format("config changed %d, change offset %ld, writer started %d")  %
        config_changed % change_offset % d_file_writer->is_started());
      if (config_changed and (change_offset == starting_offset) and not d_file_writer->is_started()) {
        GR_LOG_DEBUG(d_logger,"issuing start command");
        d_recording_changed = true;
        // d_issue_start = true;
        ntoconsume = noutput_items;
      }

      // The command to stop the writer is issued when any tags that require
      // reconfiguration are observed while the writer is started
      if (config_changed and d_file_writer->is_started()) {
        GR_LOG_DEBUG(d_logger,"issuing stop command");
        do_stop = true;
        ntoconsume = change_offset - starting_offset;
      }

      // the number of items that can be consumed is equal to the offset of the
      // last configuration change tag, which defaults to the last sample so that
      // all are consumed if no configuration changes are observed
      GR_LOG_DEBUG(d_logger,boost::format("Number of samples to consume: %ld") % (ntoconsume));
      return ntoconsume;
    }

    /***************************************************************************
     * Overloaded functions
     * ************************************************************************/
    void
    file_sink_impl::set_recording(bool state)
    {
      boost::recursive_mutex::scoped_lock lock(d_mutex);
      do_set_recording(state);
    }
    void file_sink_impl::do_set_recording(bool state)
    {
      if (d_recording && not state)
      {
        // stop recording
        this->stop();
        d_recording = false;
      }
      else if (not d_recording && state)
      {
        // start recording
        d_issue_start = true;
        d_recording = true;
        d_recording_changed = true;
      }
    }

    bool
    file_sink_impl::get_recording()
    {
      return d_recording;
    }

    void
    file_sink_impl::set_mode(trigger_type_t mode)
    {
      boost::recursive_mutex::scoped_lock lock(d_mutex);
      do_set_mode(mode);
    }
    void
    file_sink_impl::do_set_mode(trigger_type_t mode)
    {
      GR_LOG_DEBUG(d_logger, boost::format("Setting mode to %d") % mode);

      // make change only if changing mode
      if (mode != d_mode)
      {
        // ensure we're no longer recording
        do_set_recording(false);

        // update mode
        d_mode = mode;

        // reset burst state
        d_burst_state = 0;
      }
    }

    trigger_type_t
    file_sink_impl::get_mode()
    {
      return d_mode;
    }

    void
    file_sink_impl::set_gen_new_folder(bool value)
    {
      d_file_writer->set_gen_new_folder(value);
    }

  } /* namespace sandia_utils */
} /* namespace gr */
