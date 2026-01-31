#include "../common/config.hpp"
#include "../common/market_data.hpp"
#include "../common/logger.hpp"
#include "../common/ring_buffer.hpp"
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

namespace {
std::atomic<bool> running{true};

void signal_handler(int) {
    running.store(false);
}
}

class ShmConsumer {
private:
    market::RingBuffer ring_buffer_;

public:
    ShmConsumer() : ring_buffer_(false) {  // Don't create, just open
        market::Logger::log("SHM Consumer started, connected to {}", market::SHM_NAME);
    }

    void run() {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        size_t msg_count = 0;
        size_t empty_count = 0;

        while (running.load()) {
            market::MarketData data;

            if (ring_buffer_.pop(data)) {
                int64_t now = market::get_timestamp_ns();
                int64_t latency_ns = now - data.timestamp_ns;

                market::Logger::log("{} BID={:.2f} ASK={:.2f} LATENCY={}ns",
                                   data.instrument, data.bid, data.ask, latency_ns);

                msg_count++;
                empty_count = 0;

                if (msg_count % 10000 == 0) {
                    market::Logger::log("Received {} messages", msg_count);
                }
            } else {
                empty_count++;
                // Adaptive backoff when buffer is empty
                if (empty_count < 100) {
                    // Spin for a bit
                    std::this_thread::yield();
                } else {
                    // Sleep briefly to avoid burning CPU
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                }
            }
        }

        market::Logger::log("SHM Consumer shutting down. Total messages: {}", msg_count);
    }
};

int main() {
    try {
        // Small delay to ensure publisher creates shared memory first
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ShmConsumer consumer;
        consumer.run();
    } catch (const std::exception& e) {
        market::Logger::log("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
