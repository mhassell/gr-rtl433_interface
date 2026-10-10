/* -*- c++ -*- */
/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_RTL433_INTERFACE_RING_BUFFER_H
#define INCLUDED_RTL433_INTERFACE_RING_BUFFER_H

#include <gnuradio/gr_complex.h>

#include <cstddef>
#include <vector>

class RingBuffer
{
public:
    explicit RingBuffer(std::size_t capacity);

    std::size_t capacity() const;
    std::size_t size() const;
    bool empty() const;
    bool full() const;
    std::size_t push(const gr_complex* samples, std::size_t count);
    std::size_t peek(gr_complex* samples, std::size_t count) const;
    void skip(std::size_t count);
    std::size_t drain(gr_complex* samples, std::size_t count);
    void clear();

private:
    std::vector<gr_complex> d_buffer;
    std::size_t d_head = 0;
    std::size_t d_size = 0;
};

#endif /* INCLUDED_RTL433_INTERFACE_RING_BUFFER_H */
