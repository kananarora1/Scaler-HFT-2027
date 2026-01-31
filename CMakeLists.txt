cmake_minimum_required(VERSION 3.15)
project(MarketDataSystem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -ffast-math")

# Find dependencies
find_package(Boost REQUIRED COMPONENTS system)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)

# Common library
add_library(market_common STATIC
    common/market_data.cpp
    common/logger.cpp
    common/ring_buffer.cpp
)

target_include_directories(market_common PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(market_common PUBLIC
    fmt::fmt
    nlohmann_json::nlohmann_json
    pthread
    rt
)

# Publisher
add_executable(publisher publisher/main.cpp)
target_link_libraries(publisher PRIVATE
    market_common
    Boost::system
)

# SHM Consumer
add_executable(shm_consumer shm_consumer/main.cpp)
target_link_libraries(shm_consumer PRIVATE market_common)

# TCP Consumer
add_executable(tcp_consumer tcp_consumer/main.cpp)
target_link_libraries(tcp_consumer PRIVATE
    market_common
    Boost::system
)

# Fast SHM Consumer (reduced logging)
add_executable(shm_consumer_fast shm_consumer/main_fast.cpp)
target_link_libraries(shm_consumer_fast PRIVATE market_common)

# Fast TCP Consumer (reduced logging)
add_executable(tcp_consumer_fast tcp_consumer/main_fast.cpp)
target_link_libraries(tcp_consumer_fast PRIVATE
    market_common
    Boost::system
)

# Install
install(TARGETS publisher shm_consumer tcp_consumer shm_consumer_fast tcp_consumer_fast
    RUNTIME DESTINATION bin
)
