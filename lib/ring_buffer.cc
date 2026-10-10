/* -*- c++ -*- */
/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ring_buffer.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

RingBuffer::RingBuffer(std::size_t capacity) : d_buffer(capacity)
{
    if (capacity == 0)
        throw std::invalid_argument("RingBuffer capacity must be greater than zero");
}

std::size_t RingBuffer::capacity() const
{
    return d_buffer.size();
}

std::size_t RingBuffer::size() const
{
    return d_size;
}

bool RingBuffer::empty() const
{
    return d_size == 0;
}

bool RingBuffer::full() const
{
    return d_size == capacity();
}

std::size_t RingBuffer::push(const gr_complex* samples, std::size_t count)
{
    const std::size_t written = std::min(count, capacity() - d_size);
    const std::size_t tail = (d_head + d_size) % capacity();
    const std::size_t first = std::min(written, capacity() - tail);
    if (first)
        std::memcpy(d_buffer.data() + tail, samples, first * sizeof(gr_complex));
    if (written > first)
        std::memcpy(d_buffer.data(), samples + first, (written - first) * sizeof(gr_complex));
    d_size += written;
    return written;
}

std::size_t RingBuffer::peek(gr_complex* samples, std::size_t count) const
{
    const std::size_t copied = std::min(count, d_size);
    const std::size_t first = std::min(copied, capacity() - d_head);
    if (first)
        std::memcpy(samples, d_buffer.data() + d_head, first * sizeof(gr_complex));
    if (copied > first)
        std::memcpy(samples + first, d_buffer.data(), (copied - first) * sizeof(gr_complex));
    return copied;
}

void RingBuffer::skip(std::size_t count)
{
    const std::size_t skipped = std::min(count, d_size);
    d_head = (d_head + skipped) % capacity();
    d_size -= skipped;
}

std::size_t RingBuffer::drain(gr_complex* samples, std::size_t count)
{
    const std::size_t copied = peek(samples, count);
    skip(copied);
    return copied;
}

void RingBuffer::clear()
{
    d_head = 0;
    d_size = 0;
}
