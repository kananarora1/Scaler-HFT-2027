#include "../common/config.hpp"
#include "../common/market_data.hpp"
#include "../common/logger.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

using boost::asio::ip::tcp;

namespace {
std::atomic<bool> running{true};

void signal_handler(int) {
    running.store(false);
}
}

class TcpConsumer {
private:
    boost::asio::io_context io_context_;
    tcp::socket socket_;
    boost::asio::streambuf buffer_;

public:
    TcpConsumer() : socket_(io_context_) {
        market::Logger::log("TCP Consumer started");
    }

    void connect() {
        tcp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(market::TCP_HOST,
                                         std::to_string(market::TCP_PORT));

        boost::asio::connect(socket_, endpoints);

        // Optimize TCP socket
        socket_.set_option(tcp::no_delay(true));  // Disable Nagle's algorithm
        socket_.set_option(boost::asio::socket_base::receive_buffer_size(65536));

        market::Logger::log("Connected to {}:{}", market::TCP_HOST, market::TCP_PORT);
    }

    void run() {
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Retry connection until successful
        while (running.load()) {
            try {
                connect();
                break;
            } catch (const std::exception& e) {
                market::Logger::log("Connection failed: {}, retrying...", e.what());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        if (!running.load()) {
            return;
        }

        size_t msg_count = 0;

        while (running.load()) {
            try {
                // Read line (JSON message ending with \n)
                boost::asio::read_until(socket_, buffer_, '\n');

                std::istream is(&buffer_);
                std::string json_str;
                std::getline(is, json_str);

                // Parse and log
                auto data = market::MarketData::from_json(json_str);
                int64_t now = market::get_timestamp_ns();
                int64_t latency_ns = now - data.timestamp_ns;

                market::Logger::log("{} BID={:.2f} ASK={:.2f} LATENCY={}ns",
                                   data.instrument, data.bid, data.ask, latency_ns);

                msg_count++;

                if (msg_count % 10000 == 0) {
                    market::Logger::log("Received {} messages", msg_count);
                }

            } catch (const std::exception& e) {
                if (running.load()) {
                    market::Logger::log("Read error: {}", e.what());
                }
                break;
            }
        }

        market::Logger::log("TCP Consumer shutting down. Total messages: {}", msg_count);
    }
};

int main() {
    try {
        TcpConsumer consumer;
        consumer.run();
    } catch (const std::exception& e) {
        market::Logger::log("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
