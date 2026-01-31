# Makefile for Market Data System

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -flto -ffast-math
LDFLAGS = -pthread -lrt -lfmt -lboost_system

# Directories
COMMON_DIR = common
PUBLISHER_DIR = publisher
SHM_CONSUMER_DIR = shm_consumer
TCP_CONSUMER_DIR = tcp_consumer
BUILD_DIR = build

# Common sources
COMMON_SRCS = $(COMMON_DIR)/market_data.cpp $(COMMON_DIR)/logger.cpp $(COMMON_DIR)/ring_buffer.cpp
COMMON_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(COMMON_SRCS))

# Targets
TARGETS = $(BUILD_DIR)/publisher $(BUILD_DIR)/shm_consumer $(BUILD_DIR)/tcp_consumer \
          $(BUILD_DIR)/shm_consumer_fast $(BUILD_DIR)/tcp_consumer_fast

.PHONY: all clean directories

all: directories $(TARGETS)

directories:
	@mkdir -p $(BUILD_DIR)/$(COMMON_DIR)
	@mkdir -p $(BUILD_DIR)/$(PUBLISHER_DIR)
	@mkdir -p $(BUILD_DIR)/$(SHM_CONSUMER_DIR)
	@mkdir -p $(BUILD_DIR)/$(TCP_CONSUMER_DIR)

# Common objects
$(BUILD_DIR)/$(COMMON_DIR)/%.o: $(COMMON_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Publisher
$(BUILD_DIR)/$(PUBLISHER_DIR)/main.o: $(PUBLISHER_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/publisher: $(COMMON_OBJS) $(BUILD_DIR)/$(PUBLISHER_DIR)/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# SHM Consumer
$(BUILD_DIR)/$(SHM_CONSUMER_DIR)/main.o: $(SHM_CONSUMER_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/shm_consumer: $(COMMON_OBJS) $(BUILD_DIR)/$(SHM_CONSUMER_DIR)/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# TCP Consumer
$(BUILD_DIR)/$(TCP_CONSUMER_DIR)/main.o: $(TCP_CONSUMER_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tcp_consumer: $(COMMON_OBJS) $(BUILD_DIR)/$(TCP_CONSUMER_DIR)/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Fast SHM Consumer
$(BUILD_DIR)/$(SHM_CONSUMER_DIR)/main_fast.o: $(SHM_CONSUMER_DIR)/main_fast.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/shm_consumer_fast: $(COMMON_OBJS) $(BUILD_DIR)/$(SHM_CONSUMER_DIR)/main_fast.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Fast TCP Consumer
$(BUILD_DIR)/$(TCP_CONSUMER_DIR)/main_fast.o: $(TCP_CONSUMER_DIR)/main_fast.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tcp_consumer_fast: $(COMMON_OBJS) $(BUILD_DIR)/$(TCP_CONSUMER_DIR)/main_fast.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

install: all
	@mkdir -p /usr/local/bin
	cp $(TARGETS) /usr/local/bin/
