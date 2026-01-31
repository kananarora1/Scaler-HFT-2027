#pragma once

#include "config.hpp"
#include "market_data.hpp"
#include <atomic>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace market {

// Lock-free SPSC Ring Buffer using shared memory
class RingBuffer {
public:
    struct SharedMemory {
        alignas(CACHE_LINE_SIZE) std::atomic<size_t> write_idx;
        alignas(CACHE_LINE_SIZE) std::atomic<size_t> read_idx;
        alignas(CACHE_LINE_SIZE) MarketData buffer[RING_BUFFER_SIZE];
    };

private:
    int shm_fd_;
    SharedMemory* shm_;
    bool is_creator_;

public:
    RingBuffer(bool create);
    ~RingBuffer();

    // Delete copy/move
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    // Producer: push data (returns false if full)
    bool push(const MarketData& data);

    // Consumer: pop data (returns false if empty)
    bool pop(MarketData& data);

    // Get buffer usage
    size_t size() const;
};

}  // namespace market
