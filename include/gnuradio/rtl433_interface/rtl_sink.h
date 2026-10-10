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

    /*!
     * \brief <+description of block+>
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
      static sptr make();
    };

  } // namespace rtl433_interface
} // namespace gr

#endif /* INCLUDED_RTL433_INTERFACE_RTL_SINK_H */
