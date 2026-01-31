#include "../common/config.hpp"
#include "../common/market_data.hpp"
#include "../common/logger.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>
#include <vector>

using boost::asio::ip::tcp;

namespace {
std::atomic<bool> running{true};

void signal_handler(int) {
    running.store(false);
}
}

class FastTcpConsumer {
private:
    boost::asio::io_context io_context_;
    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::vector<int64_t> latencies_;

public:
    FastTcpConsumer() : socket_(io_context_) {
        latencies_.reserve(1000000);
        market::Logger::log("Fast TCP Consumer started");
    }

    void connect() {
        tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(market::TCP_HOST,
                                         std::to_string(market::TCP_PORT));

        boost::asio::connect(socket_, endpoints);
        socket_.set_option(tcp::no_delay(true));
        socket_.set_option(boost::asio::socket_base::receive_buffer_size(65536));

        market::Logger::log("Connected to {}:{}", market::TCP_HOST, market::TCP_PORT);
    }

    void run() {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        while (running.load()) {
            try {
                connect();
                break;
            } catch (const std::exception& e) {
                market::Logger::log("Connection failed: {}, retrying...", e.what());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        if (!running.load()) return;

        size_t msg_count = 0;

        while (running.load()) {
            try {
                boost::asio::read_until(socket_, buffer_, '\n');

                std::istream is(&buffer_);
                std::string json_str;
                std::getline(is, json_str);

                auto data = market::MarketData::from_json(json_str);
                int64_t now = market::get_timestamp_ns();
                int64_t latency_ns = now - data.timestamp_ns;

                latencies_.push_back(latency_ns);
                msg_count++;

                if (msg_count % 10000 == 0) {
                    market::Logger::log("Received {} messages, last latency: {}ns",
                                       msg_count, latency_ns);
                }

            } catch (const std::exception& e) {
                if (running.load()) {
                    market::Logger::log("Read error: {}", e.what());
                }
                break;
            }
        }

        market::Logger::log("Fast TCP Consumer shutting down. Total messages: {}", msg_count);
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
        FastTcpConsumer consumer;
        consumer.run();
    } catch (const std::exception& e) {
        market::Logger::log("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
