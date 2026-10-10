/* -*- c++ -*- */
/*
 * Copyright 2026 None.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RTL433_INTERFACE_RTL_SINK_H
#define INCLUDED_RTL433_INTERFACE_RTL_SINK_H

#include <gnuradio/rtl433_interface/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace rtl433_interface {

    enum sample_format_t { FMT_CU8 = 0, FMT_CS16 = 1 };

    /*!
     * \brief Decode complex baseband samples with rtl_433 and print events as KV.
     * \ingroup rtl433_interface
     *
     */
    class RTL433_INTERFACE_API rtl_sink : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<rtl_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of rtl433_interface::rtl_sink.
       *
       * To avoid accidental use of raw pointers, rtl433_interface::rtl_sink's
       * constructor is in a private implementation
       * class. rtl433_interface::rtl_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(double freq = 433920000.0,
                       double samp_rate = 250000.0,
                       sample_format_t format = FMT_CS16);

      virtual void set_freq(double freq) = 0;
      virtual void set_samp_rate(double samp_rate) = 0;
    };

  } // namespace rtl433_interface
} // namespace gr

#endif /* INCLUDED_RTL433_INTERFACE_RTL_SINK_H */
