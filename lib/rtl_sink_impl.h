/* -*- c++ -*- */
/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RTL433_INTERFACE_RTL_SINK_IMPL_H
#define INCLUDED_RTL433_INTERFACE_RTL_SINK_IMPL_H

#include <gnuradio/thread/thread.h>
#include <gnuradio/rtl433_interface/rtl_sink.h>

#include "ring_buffer.h"

#include <cstdint>
#include <mutex>
#include <vector>

struct r_cfg;

namespace gr {
namespace rtl433_interface {

class rtl_sink_impl : public rtl_sink
{
public:
    rtl_sink_impl(double freq, double samp_rate, sample_format_t format);
    ~rtl_sink_impl() override;

    void set_freq(double freq) override;
    void set_samp_rate(double samp_rate) override;
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

private:
    static const std::size_t FRAME_BYTES = 16 * 16384;

    void update_frequency(double freq);
    void update_sample_rate(double samp_rate);
    void convert_frame();

    double d_freq;
    double d_samp_rate;
    sample_format_t d_format;
    std::size_t d_bytes_per_sample;
    r_cfg* d_cfg;
    std::size_t d_frame_samples;
    RingBuffer d_ring;
    std::vector<gr_complex> d_frame_scratch;
    alignas(4) std::vector<std::uint8_t> d_convbuf;
    gr::thread::mutex d_setlock;
};

} // namespace rtl433_interface
} // namespace gr

#endif /* INCLUDED_RTL433_INTERFACE_RTL_SINK_IMPL_H */
