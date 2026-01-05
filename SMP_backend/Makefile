# Makefile for Mini Instagram Social Media Platform
# Author: Furqan-2006
# Project: DS-Project-2025

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -I include
DEBUGFLAGS := -g -DDEBUG
RELEASEFLAGS := -O2 -DNDEBUG

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include
DATA_DIR := data

# Source files
SOURCES := $(SRC_DIR)/main.cpp \
           $(SRC_DIR)/content/feed.cpp \
           $(SRC_DIR)/content/post.cpp \
           $(SRC_DIR)/content/recommendation.cpp \
           $(SRC_DIR)/core/followerList.cpp \
           $(SRC_DIR)/core/relationGraph.cpp \
           $(SRC_DIR)/core/user.cpp \
           $(SRC_DIR)/core/status.cpp \
           $(SRC_DIR)/interaction/message.cpp \
           $(SRC_DIR)/interaction/friendRequest.cpp \
           $(SRC_DIR)/system/notification.cpp \
           $(SRC_DIR)/system/search.cpp \
           $(SRC_DIR)/system/systemManager.cpp \
           $(SRC_DIR)/utils/fileIO.cpp \
           $(SRC_DIR)/utils/helpers.cpp \
           $(SRC_DIR)/utils/validation.cpp

# Object files
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Executable name
TARGET := $(BUILD_DIR)/Mini_Instagram

# Default target
.PHONY: all
all: release

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/content
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/interaction
	@mkdir -p $(BUILD_DIR)/system
	@mkdir -p $(BUILD_DIR)/utils

# Release build
.PHONY: release
release: CXXFLAGS += $(RELEASEFLAGS)
release: $(BUILD_DIR) $(TARGET)
	@echo "✓ Release build completed successfully!"
	@echo "  Run with: ./$(TARGET)"

# Debug build
.PHONY: debug
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(BUILD_DIR) $(TARGET)
	@echo "✓ Debug build completed successfully!"
	@echo "  Run with: ./$(TARGET)"

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking executable..."
	@$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)/*.o
	@rm -rf $(BUILD_DIR)/content/*.o
	@rm -rf $(BUILD_DIR)/core/*.o
	@rm -rf $(BUILD_DIR)/interaction/*.o
	@rm -rf $(BUILD_DIR)/system/*.o
	@rm -rf $(BUILD_DIR)/utils/*.o
	@rm -f $(TARGET)
	@echo "✓ Clean completed!"

# Deep clean (including data files)
.PHONY: distclean
distclean: clean
	@echo "Performing deep clean..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Deep clean completed!"

# Run the application
.PHONY: run
run: release
	@echo "Running Mini Instagram..."
	@./$(TARGET)

# Run in debug mode
.PHONY: run-debug
run-debug: debug
	@echo "Running Mini Instagram (debug mode)..."
	@gdb ./$(TARGET)

# Check for memory leaks (requires valgrind)
.PHONY: memcheck
memcheck: debug
	@echo "Running memory leak check..."
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Static analysis (requires cppcheck)
.PHONY: analyze
analyze:
	@echo "Running static analysis..."
	@cppcheck --enable=all --std=c++17 --inconclusive \
		--suppress=missingIncludeSystem \
		-I $(INCLUDE_DIR) $(SRC_DIR)

# Format code (requires clang-format)
.PHONY: format
format:
	@echo "Formatting code..."
	@find $(SRC_DIR) $(INCLUDE_DIR) -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
	@echo "✓ Code formatting completed!"

# Display help
.PHONY: help
help:
	@echo "Mini Instagram - Build System"
	@echo "=============================="
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build release version (default)"
	@echo "  make release      - Build optimized release version"
	@echo "  make debug        - Build debug version with symbols"
	@echo "  make clean        - Remove object files and executable"
	@echo "  make distclean    - Remove all build artifacts"
	@echo "  make run          - Build and run release version"
	@echo "  make run-debug    - Build and run with gdb debugger"
	@echo "  make memcheck     - Run with valgrind memory checker"
	@echo "  make analyze      - Run static code analysis (cppcheck)"
	@echo "  make format       - Format code with clang-format"
	@echo "  make help         - Display this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make clean && make release"
	@echo "  make debug && make run-debug"

# Dependencies (simplified - for production use proper dependency generation)
-include $(OBJECTS:.o=.d)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@

.PHONY: deps
deps:
	@echo "Generating dependencies..."
	@$(MAKE) -s $(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJECTS))
