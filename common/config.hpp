#pragma once

#include <cstddef>
#include <cstdint>

namespace market {

// TCP Configuration
constexpr const char* TCP_HOST = "127.0.0.1";
constexpr uint16_t TCP_PORT = 9000;

// Shared Memory Configuration
constexpr const char* SHM_NAME = "/market_data_shm";
constexpr size_t RING_BUFFER_SIZE = 4096;  // Must be power of 2

// Publishing Configuration
constexpr size_t MESSAGES_PER_SECOND = 10000;
constexpr const char* INSTRUMENT = "RELIANCE";

// Cache line size for alignment
constexpr size_t CACHE_LINE_SIZE = 64;

}  // namespace market
