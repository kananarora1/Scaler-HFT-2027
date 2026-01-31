#include "../common/config.hpp"
#include "../common/market_data.hpp"
#include "../common/logger.hpp"
#include "../common/ring_buffer.hpp"
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <vector>

namespace {
std::atomic<bool> running{true};

void signal_handler(int) {
    running.store(false);
}
}

class FastShmConsumer {
private:
    market::RingBuffer ring_buffer_;
    std::vector<int64_t> latencies_;

public:
    FastShmConsumer() : ring_buffer_(false) {
        latencies_.reserve(1000000);
        market::Logger::log("Fast SHM Consumer started, connected to {}", market::SHM_NAME);
    }

    void run() {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        size_t msg_count = 0;

        while (running.load()) {
            market::MarketData data;

            if (ring_buffer_.pop(data)) {
                int64_t now = market::get_timestamp_ns();
                int64_t latency_ns = now - data.timestamp_ns;

                // Store latency for analysis (don't log every message)
                latencies_.push_back(latency_ns);

                msg_count++;

                // Log every 10,000 messages instead of every message
                if (msg_count % 10000 == 0) {
                    market::Logger::log("Received {} messages, last latency: {}ns",
                                       msg_count, latency_ns);
                }
            } else {
                // Spin without sleeping for maximum performance
                std::this_thread::yield();
            }
        }

        market::Logger::log("Fast SHM Consumer shutting down. Total messages: {}", msg_count);
        print_statistics();
    }

private:
    void print_statistics() {
        if (latencies_.empty()) {
            market::Logger::log("No latency data collected");
            return;
        }

        std::sort(latencies_.begin(), latencies_.end());

        int64_t min = latencies_.front();
        int64_t max = latencies_.back();
        int64_t median = latencies_[latencies_.size() / 2];
        int64_t p95 = latencies_[latencies_.size() * 95 / 100];
        int64_t p99 = latencies_[latencies_.size() * 99 / 100];

        int64_t sum = 0;
        for (auto lat : latencies_) sum += lat;
        int64_t mean = sum / latencies_.size();

        market::Logger::log("=== Latency Statistics ===");
        market::Logger::log("Count:  {}", latencies_.size());
        market::Logger::log("Min:    {} ns", min);
        market::Logger::log("Median: {} ns ({:.2f} μs)", median, median / 1000.0);
        market::Logger::log("Mean:   {} ns ({:.2f} μs)", mean, mean / 1000.0);
        market::Logger::log("P95:    {} ns ({:.2f} μs)", p95, p95 / 1000.0);
        market::Logger::log("P99:    {} ns ({:.2f} μs)", p99, p99 / 1000.0);
        market::Logger::log("Max:    {} ns ({:.2f} μs)", max, max / 1000.0);
    }
};

int main() {
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FastShmConsumer consumer;
        consumer.run();
    } catch (const std::exception& e) {
        market::Logger::log("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
