#include "ring_buffer.h"
#include <stdexcept>

void RingBuffer::initialize(unsigned int BUF_SIZE)
{
    if (BUF_SIZE == 0) {
        throw std::invalid_argument("RingBuffer: BUF_SIZE must be greater than zero");
    }
    d_buffer.assign(BUF_SIZE, gr_complex(0.0f, 0.0f));
    d_capacity = BUF_SIZE;
    d_head = d_tail = d_count = 0;
    d_initialized = true;
}

void RingBuffer::add(const gr_complex& value)
{
    if (!d_initialized) return;
    d_buffer[d_tail] = value;
    d_tail = (d_tail + 1) % d_capacity;
    if (d_count == d_capacity) {
        d_head = (d_head + 1) % d_capacity;
    } else {
        d_count++;
    }
}

gr_complex RingBuffer::read()
{
    if (!d_initialized || d_count == 0) return gr_complex(0.0f, 0.0f);
    gr_complex value = d_buffer[d_head];
    d_head = (d_head + 1) % d_capacity;
    d_count--;
    return value;
}

unsigned int RingBuffer::drainToCU8(uint8_t* dst, unsigned int max_count)
{
    if (!d_initialized || dst == nullptr) return 0;

    unsigned int n = (d_count < max_count) ? d_count : max_count;
    for (unsigned int k = 0; k < n; ++k) {
        const gr_complex& s = d_buffer[d_head];
        // float [-1,1] -> uint8 [0,255], 128 = zero (rtl_433 CU8 convention)
        dst[2 * k]     = (uint8_t)(s.real() * 127.0f + 128.0f);
        dst[2 * k + 1] = (uint8_t)(s.imag() * 127.0f + 128.0f);
        d_head = (d_head + 1) % d_capacity;
    }
    d_count -= n;
    return n;
}
