/* -*- c++ -*- */
/*
 * Copyright 2026 None.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "rtl_sink_impl.h"

namespace gr {
  namespace rtl433_interface {

    using input_type = gr_complex;
    rtl_sink::sptr
    rtl_sink::make()
    {
      return gnuradio::make_block_sptr<rtl_sink_impl>(
        );
    }


    /*
     * The private constructor
     */
    rtl_sink_impl::rtl_sink_impl()
      : gr::sync_block("rtl_sink",
              gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(0, 0, 0))
    {
       // TODO RingBuf default size is default IQ buf size in rtl_433.  Should the user be allowed to modify?
       ringbuf = RingBuffer();
       ringbuf.initialize(16 * 32 * 512 / 2);
       cfg = r_create_cfg();
       r_init_cfg(cfg);
       register_all_protocols(cfg, 999);

       // hard code ffor now
       set_center_freq(cfg, 433955000);
       set_sample_rate(cfg, 250000);
       cfg->demod->samp_rate  = 250000;   // process_sdr_frame normally syncs these;
       cfg->demod->center_frequency = 433955000;  // you bypass that function, so set both
       cfg->demod->sample_size = sizeof(int16_t) * 2; // CS16, CF32 (after conversion)
       add_kv_output(cfg, (char*) "-");
       cfg->demod->raw_handler = &cfg->raw_handler;
       cfg->demod->load_info.format = CF32_IQ;


    }

    /*
     * Our virtual destructor.
     */
    rtl_sink_impl::~rtl_sink_impl()
    {
    }

int 
rtl_sink_impl::work(int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items)
{
    auto in = static_cast<const gr_complex*>(input_items[0]);

    int i = 0;
    for (; i < noutput_items; ++i)
    {
        if (!ringbuf.isFull())
            ringbuf.add(in[i]);
        else
        {
            unsigned int n = ringbuf.drainToCU8(d_uintbuf.data(), d_uintbuf.size());
            if (n > 0)
            {
                std::cout << "Flushing!" << n << std::endl;
                push_sdr_flow(cfg, d_uintbuf.data(), n * 2);
            }
        }
    }

    return i;   // number of input samples consumed (no consume_each in work())
}


void rtl_sink_impl::convertFrame(const gr_complex *in, size_t n)
{
    if (d_format == FMT_CU8) {
        d_convbuf.resize(n * 2);
        uint8_t *p = d_convbuf.data();
        for (size_t i = 0; i < n; ++i) {
            p[2*i]   = f32_to_u8(in[i].real());
            p[2*i+1] = f32_to_u8(in[i].imag());
        }
    } else {
        d_convbuf.resize(n * 4);
        int16_t *p = (int16_t *)d_convbuf.data();
        for (size_t i = 0; i < n; ++i) {
            p[2*i]   = f32_to_s16(in[i].real());
            p[2*i+1] = f32_to_s16(in[i].imag());
        }
    }
}

  } /* namespace rtl433_interface */
} /* namespace gr */
