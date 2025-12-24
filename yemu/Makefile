# Project settings
PROJECT = yemu
TARGET = $(PROJECT)

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Compiler settings
CC = gcc
CFLAGS = -I$(INC_DIR) -Wall -Werror -O2 -std=c11
LDFLAGS = 

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Target executable
TARGET_PATH = $(BUILD_DIR)/$(TARGET)

# Default target
all: $(TARGET_PATH)

# Link object files to create executable
$(TARGET_PATH): $(OBJS)
	@echo "Linking: $@"
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

# Compile source files to object files with dependency generation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# Run the program
run: $(TARGET_PATH)
	@$(TARGET_PATH)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)

# Show project structure
info:
	@echo "Project: $(PROJECT)"
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"
	@echo "Target: $(TARGET_PATH)"

# Phony targets
.PHONY: all run clean info
