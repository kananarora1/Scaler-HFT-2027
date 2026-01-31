#include "market_data.hpp"
#include <chrono>
#include <cstring>
#include <nlohmann/json.hpp>

namespace market {

MarketData::MarketData(const char* inst, double b, double a, int64_t ts)
    : bid(b), ask(a), timestamp_ns(ts) {
    std::strncpy(instrument, inst, sizeof(instrument) - 1);
    instrument[sizeof(instrument) - 1] = '\0';
}

std::string MarketData::to_json() const {
    nlohmann::json j;
    j["instrument"] = instrument;
    j["bid"] = bid;
    j["ask"] = ask;
    j["timestamp_ns"] = timestamp_ns;
    return j.dump();
}

MarketData MarketData::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    MarketData data;
    std::strncpy(data.instrument, j["instrument"].get<std::string>().c_str(),
                 sizeof(data.instrument) - 1);
    data.instrument[sizeof(data.instrument) - 1] = '\0';
    data.bid = j["bid"];
    data.ask = j["ask"];
    data.timestamp_ns = j["timestamp_ns"];
    return data;
}

int64_t get_timestamp_ns() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
}

}  // namespace market
