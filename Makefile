#Author: Richard Walmsley
#Date: 2025/03/25

# Variables
EXE = chocolate
SRC_DIR = src
INC_DIR = inc
EXTERN = extern
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/release/$(EXE)
DEBUG_TARGET = $(BIN_DIR)/debug/$(EXE)
CC = gcc
CFLAGS = -I $(INC_DIR) -Wall -Wextra

# Gather all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
INCS = $(wildcard $(INC_DIR)/*.h)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
DEBUG_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/debug/%.o, $(SRCS))

# Create the release executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)/release
	@echo "Linking: $<"
	$(CC) $(OBJS) -o $@ -lm

# Create debug executable
$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(BIN_DIR)/debug
	@echo "Linking $(DEBUG_OBJS)"
	$(CC) $(DEBUG_OBJS) -o $@ -lm

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h 
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $< into $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile source files into debug object files
$(BUILD_DIR)/debug/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	@mkdir -p $(BUILD_DIR)/debug
	@echo "Compiling $< into $@ (debug)"
	@$(CC) $(CFLAGS) -g -c $< -o $@

debug: $(DEBUG_TARGET)
	@gdb -tui $<

# Clean up
clean:
	@echo "Cleaning build and bin directories"
	@rm -rf $(BUILD_DIR)/* && rm -rf $(BIN_DIR)/*

# Run build
run: $(TARGET)
	@./$<

# Show all source files
show:
	@echo "Sources: $(SRCS)"
	@echo "Includes: $(INCS)"
	@echo "Objects: $(OBJS)"
	@echo "Debug Objects: $(DEBUG_OBJS)"

# Phony targets
.PHONY: all clean run show
