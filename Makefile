#Author: Richard Walmsley
#Date: 2025/03/25

# Variables
PROJ = chocolate
SRC_DIR = src
INC_DIR = inc
EXTERN = extern
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/release/$(PROJ)
DEBUG_TARGET = $(BIN_DIR)/debug/$(PROJ)
CC = gcc
CFLAGS = -I $(EXTERN) -Wall -Wextra

# Gather all source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
INCS = $(wildcard $(INC_DIR)/*.h)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Create the release executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BIN_DIR)/release
	@$(CC) $(CFLAGS) $< -o $@ -lm

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRCS) $(INCS)
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	@rm -rf $(BUILD_DIR)/* && rm -rf $(BIN_DIR)/*

# Run build
run: $(TARGET)
	@./$<

# Show all source files
show:
	@echo "Sources: $(SRCS)"
	@echo "Includes: $(INCS)"
	@echo "Objects: $(OBJS)"

# Phony targets
.PHONY: all clean run show