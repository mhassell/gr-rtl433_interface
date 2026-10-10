#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <gnuradio/gr_complex.h>
#include <vector>
#include <cstdint>

class RingBuffer
{
public:
    RingBuffer() = default;
    ~RingBuffer() = default;

    void initialize(unsigned int BUF_SIZE);
    void add(const gr_complex& value);
    gr_complex read();

    // Bulk drain: convert up to max_count samples to CU8 (interleaved I/Q,
    // the format rtl_433 decoders expect) into dst, which must have room
    // for 2*max_count bytes. Returns the number of samples written.
    // Handles wraparound internally — caller gets one contiguous chunk.
    unsigned int drainToCU8(uint8_t* dst, unsigned int max_count);

    bool isEmpty() const { return d_count == 0; }
    bool isFull() const { return d_count == d_capacity; }
    unsigned int size() const { return d_count; }
    unsigned int capacity() const { return d_capacity; }

private:
    std::vector<gr_complex> d_buffer;
    unsigned int d_capacity = 0;
    unsigned int d_head = 0;
    unsigned int d_tail = 0;
    unsigned int d_count = 0;
    bool d_initialized = false;
};

#endif // RING_BUFFER_H
