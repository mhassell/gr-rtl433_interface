/* -*- c++ -*- */
/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rtl_sink_impl.h"

#include <rtl_433/fileformat.h>
#include <rtl_433/pulse_detect.h>
#include <rtl_433/r_api.h>
#include <rtl_433/r_flow.h>
#include <rtl_433/r_private.h>
#include <rtl_433/rtl_433.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>

namespace gr {
namespace rtl433_interface {
namespace {

std::uint32_t checked_rate(double rate)
{
    if (!std::isfinite(rate) || rate <= 0.0 ||
        rate > static_cast<double>(std::numeric_limits<std::uint32_t>::max()))
        throw std::invalid_argument("Sample rate must be a positive 32-bit value");
    return static_cast<std::uint32_t>(rate);
}

std::uint32_t checked_frequency(double frequency)
{
    if (!std::isfinite(frequency) || frequency <= 0.0 ||
        frequency > static_cast<double>(std::numeric_limits<std::uint32_t>::max()))
        throw std::invalid_argument("Frequency must be a positive 32-bit value");
    return static_cast<std::uint32_t>(frequency);
}

float clamp_unit(float value)
{
    return std::max(-1.0f, std::min(1.0f, value));
}

} // namespace

rtl_sink::sptr rtl_sink::make(double freq, double samp_rate, sample_format_t format)
{
    return std::make_shared<rtl_sink_impl>(freq, samp_rate, format);
}

rtl_sink_impl::rtl_sink_impl(double freq, double samp_rate, sample_format_t format)
    : gr::sync_block("rtl_sink",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(0, 0, 0)),
      d_freq(freq),
      d_samp_rate(samp_rate),
      d_format(format),
      d_bytes_per_sample(format == FMT_CU8 ? 2 : 4),
      d_cfg(nullptr),
      d_frame_samples(FRAME_BYTES / d_bytes_per_sample),
      d_ring(FRAME_BYTES),
      d_frame_scratch(d_frame_samples),
      d_convbuf(FRAME_BYTES)
{
    if (format != FMT_CU8 && format != FMT_CS16)
        throw std::invalid_argument("Unsupported rtl_433 sample format");
    checked_frequency(freq);
    checked_rate(samp_rate);

    d_cfg = r_create_cfg();
    if (!d_cfg)
        throw std::runtime_error("Failed to create rtl_433 configuration");

    register_all_protocols(d_cfg, 0);
    d_cfg->demod->pulse_detect = pulse_detect_create();
    if (!d_cfg->demod->pulse_detect) {
        r_free_cfg(d_cfg);
        d_cfg = nullptr;
        throw std::runtime_error("Failed to create rtl_433 pulse detector");
    }

    d_cfg->demod->load_info.format = format == FMT_CU8 ? CU8_IQ : CS16_IQ;
    d_cfg->demod->sample_size = static_cast<int>(d_bytes_per_sample);
    update_frequency(freq);
    update_sample_rate(samp_rate);

    add_kv_output(d_cfg, const_cast<char*>("-"));
    start_outputs(d_cfg, well_known_output_fields(d_cfg));
}

rtl_sink_impl::~rtl_sink_impl()
{
    if (d_cfg)
        r_free_cfg(d_cfg);
}

void rtl_sink_impl::update_frequency(double freq)
{
    const std::uint32_t value = checked_frequency(freq);
    d_freq = freq;
    d_cfg->center_frequency = value;
    d_cfg->demod->center_frequency = value;
    d_cfg->demod->load_info.center_frequency = value;
}

void rtl_sink_impl::update_sample_rate(double samp_rate)
{
    const std::uint32_t value = checked_rate(samp_rate);
    d_samp_rate = samp_rate;
    d_cfg->samp_rate = value;
    d_cfg->demod->samp_rate = value;
    d_cfg->demod->load_info.sample_rate = value;
}

void rtl_sink_impl::set_freq(double freq)
{
    std::lock_guard<gr::thread::mutex> lock(d_setlock);
    update_frequency(freq);
    flush_sdr_flow(d_cfg); // [verify] Confirm the target library exposes this flow reset API.
}

void rtl_sink_impl::set_samp_rate(double samp_rate)
{
    std::lock_guard<gr::thread::mutex> lock(d_setlock);
    update_sample_rate(samp_rate);
    flush_sdr_flow(d_cfg); // [verify] Confirm the target library exposes this flow reset API.
}

void rtl_sink_impl::convert_frame()
{
    if (d_format == FMT_CU8) {
        for (std::size_t i = 0; i < d_frame_samples; ++i) {
            const float real = clamp_unit(d_frame_scratch[i].real());
            const float imag = clamp_unit(d_frame_scratch[i].imag());
            const long i_value = std::clamp(std::lrintf(real * 127.5f + 127.5f), 0L, 255L);
            const long q_value = std::clamp(std::lrintf(imag * 127.5f + 127.5f), 0L, 255L);
            d_convbuf[2 * i] = static_cast<std::uint8_t>(i_value);
            d_convbuf[2 * i + 1] = static_cast<std::uint8_t>(q_value);
        }
    } else {
        auto* converted = reinterpret_cast<std::int16_t*>(d_convbuf.data());
        for (std::size_t i = 0; i < d_frame_samples; ++i) {
            const float real = clamp_unit(d_frame_scratch[i].real());
            const float imag = clamp_unit(d_frame_scratch[i].imag());
            converted[2 * i] = static_cast<std::int16_t>(
                std::clamp(std::lrintf(real * 32767.0f), -32767L, 32767L));
            converted[2 * i + 1] = static_cast<std::int16_t>(
                std::clamp(std::lrintf(imag * 32767.0f), -32767L, 32767L));
        }
    }
}

int rtl_sink_impl::work(int noutput_items,
                        gr_vector_const_void_star& input_items,
                        gr_vector_void_star& output_items)
{
    (void)output_items;
    std::lock_guard<gr::thread::mutex> lock(d_setlock);
    const auto* input = static_cast<const gr_complex*>(input_items[0]);
    std::size_t consumed = 0;
    const std::size_t available = static_cast<std::size_t>(noutput_items);

    for (;;) {
        while (d_ring.size() >= d_frame_samples) {
            d_ring.drain(d_frame_scratch.data(), d_frame_samples);
            convert_frame();
            push_sdr_flow(d_cfg, d_convbuf.data(), static_cast<std::uint32_t>(FRAME_BYTES));
        }
        if (consumed >= available)
            break;
        const std::size_t pushed =
            d_ring.push(input + consumed, std::min(available - consumed, d_ring.capacity() - d_ring.size()));
        consumed += pushed;
    }

    return noutput_items;
}

} // namespace rtl433_interface
} // namespace gr
