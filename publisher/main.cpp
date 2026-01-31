#include "../common/config.hpp"
#include "../common/market_data.hpp"
#include "../common/logger.hpp"
#include "../common/ring_buffer.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <random>
#include <csignal>
#include <atomic>

using boost::asio::ip::tcp;

namespace {
std::atomic<bool> running{true};

void signal_handler(int) {
    running.store(false);
}
}

class Publisher {
private:
    boost::asio::io_context io_context_;
    tcp::acceptor acceptor_;
    std::unique_ptr<tcp::socket> socket_;
    market::RingBuffer ring_buffer_;

    std::mt19937 rng_;
    std::uniform_real_distribution<double> price_dist_{2800.0, 2900.0};
    std::uniform_real_distribution<double> spread_dist_{0.25, 1.0};

public:
    Publisher()
        : acceptor_(io_context_, tcp::endpoint(tcp::v4(), market::TCP_PORT)),
          ring_buffer_(true),  // Create shared memory
          rng_(std::random_device{}()) {

        market::Logger::log("Publisher started on {}:{}", market::TCP_HOST, market::TCP_PORT);
        market::Logger::log("Shared memory created: {}", market::SHM_NAME);
        market::Logger::log("Target rate: {} msg/sec", market::MESSAGES_PER_SECOND);
    }

    void accept_connection() {
        socket_ = std::make_unique<tcp::socket>(io_context_);
        acceptor_.accept(*socket_);

        // Optimize TCP socket
        socket_->set_option(tcp::no_delay(true));  // Disable Nagle's algorithm
        socket_->set_option(boost::asio::socket_base::send_buffer_size(65536));

        market::Logger::log("TCP client connected");
    }

    void run() {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Accept connection in background
        std::thread accept_thread([this]() {
            try {
                accept_connection();
            } catch (const std::exception& e) {
                market::Logger::log("Accept error: {}", e.what());
            }
        });

        // Publishing loop
        auto interval = std::chrono::nanoseconds(1'000'000'000 / market::MESSAGES_PER_SECOND);
        auto next_publish = std::chrono::steady_clock::now();

        size_t msg_count = 0;

        while (running.load()) {
            // Generate market data
            double bid = price_dist_(rng_);
            double spread = spread_dist_(rng_);
            double ask = bid + spread;
            int64_t timestamp = market::get_timestamp_ns();

            market::MarketData data(market::INSTRUMENT, bid, ask, timestamp);

            // Publish to shared memory
            if (!ring_buffer_.push(data)) {
                market::Logger::log("WARNING: Ring buffer full, message dropped");
            }

            // Publish to TCP
            if (socket_ && socket_->is_open()) {
                try {
                    std::string json = data.to_json() + "\n";
                    boost::asio::write(*socket_, boost::asio::buffer(json));
                } catch (const std::exception& e) {
                    market::Logger::log("TCP send error: {}", e.what());
                    socket_.reset();
                }
            }

            msg_count++;
            if (msg_count % 10000 == 0) {
                market::Logger::log("Published {} messages, ring buffer size: {}",
                                   msg_count, ring_buffer_.size());
            }

            // Wait until next publish time
            next_publish += interval;
            std::this_thread::sleep_until(next_publish);
        }

        market::Logger::log("Publisher shutting down. Total messages: {}", msg_count);

        if (accept_thread.joinable()) {
            accept_thread.join();
        }
    }
};

int main() {
    try {
        Publisher publisher;
        publisher.run();
    } catch (const std::exception& e) {
        market::Logger::log("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
