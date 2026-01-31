#pragma once

#include <cstdint>
#include <string>

namespace market {

struct MarketData {
    char instrument[16];
    double bid;
    double ask;
    int64_t timestamp_ns;

    MarketData() = default;
    MarketData(const char* inst, double b, double a, int64_t ts);

    // Serialize to JSON string
    std::string to_json() const;

    // Deserialize from JSON string
    static MarketData from_json(const std::string& json);
};

// Get current time in nanoseconds
int64_t get_timestamp_ns();

}  // namespace market
