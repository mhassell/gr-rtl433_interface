/* -*- c++ -*- */
/*
 * Copyright 2026 None.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RTL433_INTERFACE_RTL_SINK_IMPL_H
#define INCLUDED_RTL433_INTERFACE_RTL_SINK_IMPL_H

#include <gnuradio/rtl433_interface/rtl_sink.h>
#include <vector>

#include "ring_buffer.h"
extern "C"
{
#include "rtl_433/r_api.h"
#include "rtl_433/rtl_433.h"
#include "rtl_433/r_flow.h"
#include "rtl_433/r_private.h"
}

namespace gr {
  namespace rtl433_interface {

    class rtl_sink_impl : public rtl_sink
    {
     private:
      // Nothing to declare in this block.

     public:
      rtl_sink_impl();
      ~rtl_sink_impl();
      RingBuffer ringbuf;
      struct r_cfg* cfg;
      std::vector<uint8_t> d_uintbuf = std::vector<uint8_t>(16 * 32 * 512);
      static uint8_t f32_to_u8(float f);
      static int16_t  f32_to_s16(float f);
      void convertFrame(const gr_complex *in, size_t n);

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace rtl433_interface
} // namespace gr

#endif /* INCLUDED_RTL433_INTERFACE_RTL_SINK_IMPL_H */
