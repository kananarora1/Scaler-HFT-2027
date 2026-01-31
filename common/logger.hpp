#pragma once

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <chrono>

namespace market {

class Logger {
public:
    template<typename... Args>
    static void log(fmt::format_string<Args...> format_str, Args&&... args) {
        auto now = std::chrono::system_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()) % 1000000000;

        auto time = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time);

        fmt::print("[{:02d}:{:02d}:{:02d}.{:09d}] ",
                   tm.tm_hour, tm.tm_min, tm.tm_sec, ns.count());
        fmt::print(format_str, std::forward<Args>(args)...);
        fmt::print("\n");
    }
};

}  // namespace market
