CXX ?= g++
CC ?= gcc

MODE ?= release

CXXFLAGS_DEFAULT = -std=c++20 -Wall -Wextra -Wpedantic
CCFLAGS_DEFAULT = -std=c99 -Wall -Wextra -Wpedantic
CXXFLAGS_RELEASE = $(CXXFLAGS_DEFAULT) -O3 -flto=auto -static -s -fomit-frame-pointer \
    -fno-stack-protector -fno-stack-limit -fno-semantic-interposition -march=native -mtune=native
CXXFLAGS_DEBUG   = $(CXXFLAGS_DEFAULT) -O0 -g3 -fanalyzer -fsanitize=address,undefined \
    -fno-omit-frame-pointer

ifeq ($(MODE), debug)
    CXXFLAGS = $(CXXFLAGS_DEBUG)
else
    CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

SRC_DIR   = ./src
OBJ_DIR   = ./obj
BIN_DIR   = ./bin
TEST_DIR  = ./tests
GTEST_DIR = ./tests/googletest-1.18.0/googletest

CORE_OBJS = $(OBJ_DIR)/arithmetic.o $(OBJ_DIR)/predictor.o $(OBJ_DIR)/encoder.o \
            $(OBJ_DIR)/compressor.o $(OBJ_DIR)/decompressor.o
OBJS      = $(CORE_OBJS) $(OBJ_DIR)/main.o

TARGET      = $(BIN_DIR)/pa3q
TEST_TARGET = $(BIN_DIR)/pa3q_tests

.PHONY: all prepare clean debug test

all: prepare $(TARGET)

debug:
	@$(MAKE) MODE=debug

prepare:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CCFLAGS) -I$(SRC_DIR) -c $< -o $@


$(OBJ_DIR)/gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/include -I$(GTEST_DIR) -c $< -o $@

$(OBJ_DIR)/gtest_main.o: $(GTEST_DIR)/src/gtest_main.cc
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/include -I$(GTEST_DIR) -c $< -o $@

test: prepare $(CORE_OBJS) $(OBJ_DIR)/gtest-all.o $(OBJ_DIR)/gtest_main.o
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(GTEST_DIR)/include $(TEST_DIR)/tests.cpp $(CORE_OBJS) $(OBJ_DIR)/gtest-all.o $(OBJ_DIR)/gtest_main.o -lpthread -o $(TEST_TARGET)
	@echo "==== RUNNING TESTS ===="
	@$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) test_input.bin test_archive.pa3q test_output.bin