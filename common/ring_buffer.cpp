#include "ring_buffer.hpp"
#include <sys/stat.h>

namespace market {

RingBuffer::RingBuffer(bool create) : shm_fd_(-1), shm_(nullptr), is_creator_(create) {
    if (create) {
        // Unlink any existing shared memory
        shm_unlink(SHM_NAME);

        // Create shared memory
        shm_fd_ = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd_ == -1) {
            throw std::runtime_error("Failed to create shared memory");
        }

        // Set size
        if (ftruncate(shm_fd_, sizeof(SharedMemory)) == -1) {
            close(shm_fd_);
            throw std::runtime_error("Failed to set shared memory size");
        }
    } else {
        // Open existing shared memory
        shm_fd_ = shm_open(SHM_NAME, O_RDWR, 0666);
        if (shm_fd_ == -1) {
            throw std::runtime_error("Failed to open shared memory");
        }
    }

    // Map shared memory
    shm_ = static_cast<SharedMemory*>(
        mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE,
             MAP_SHARED, shm_fd_, 0));

    if (shm_ == MAP_FAILED) {
        close(shm_fd_);
        throw std::runtime_error("Failed to map shared memory");
    }

    // Initialize if creator
    if (create) {
        shm_->write_idx.store(0, std::memory_order_relaxed);
        shm_->read_idx.store(0, std::memory_order_relaxed);
        std::memset(shm_->buffer, 0, sizeof(shm_->buffer));
    }
}

RingBuffer::~RingBuffer() {
    if (shm_ != nullptr && shm_ != MAP_FAILED) {
        munmap(shm_, sizeof(SharedMemory));
    }

    if (shm_fd_ != -1) {
        close(shm_fd_);
    }

    if (is_creator_) {
        shm_unlink(SHM_NAME);
    }
}

bool RingBuffer::push(const MarketData& data) {
    size_t write_idx = shm_->write_idx.load(std::memory_order_relaxed);
    size_t read_idx = shm_->read_idx.load(std::memory_order_acquire);
    size_t next_write = (write_idx + 1) & (RING_BUFFER_SIZE - 1);

    // Check if buffer is full
    if (next_write == read_idx) {
        return false;
    }

    // Write data
    shm_->buffer[write_idx] = data;

    // Update write index with release semantics
    shm_->write_idx.store(next_write, std::memory_order_release);
    return true;
}

bool RingBuffer::pop(MarketData& data) {
    size_t read_idx = shm_->read_idx.load(std::memory_order_relaxed);
    size_t write_idx = shm_->write_idx.load(std::memory_order_acquire);

    // Check if buffer is empty
    if (read_idx == write_idx) {
        return false;
    }

    // Read data
    data = shm_->buffer[read_idx];

    // Update read index with release semantics
    size_t next_read = (read_idx + 1) & (RING_BUFFER_SIZE - 1);
    shm_->read_idx.store(next_read, std::memory_order_release);
    return true;
}

size_t RingBuffer::size() const {
    size_t write_idx = shm_->write_idx.load(std::memory_order_relaxed);
    size_t read_idx = shm_->read_idx.load(std::memory_order_relaxed);
    return (write_idx - read_idx) & (RING_BUFFER_SIZE - 1);
}

}  // namespace market
