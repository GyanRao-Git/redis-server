CXX = g++
CXXFLAGS = -std=c++17 -Wall -MMD -MP -O2

SRC_DIR = src
BUILD_DIR = build

TARGET = redis-cli

# Windows-specific: link with ws2_32 for sockets
LIBS = -lws2_32

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(TARGET): $(BUILD_DIR)/main.o
	$(CXX) $(CXXFLAGS) $(BUILD_DIR)/main.o -o $(TARGET) $(LIBS)

clean:
	del /Q $(BUILD_DIR) 2>nul
	del $(TARGET) 2>nul

rebuild: clean all

run: all
	$(TARGET)

.PHONY: all clean rebuild run
