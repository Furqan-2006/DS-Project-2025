#!/bin/bash
# Quick setup and build script for Mini Instagram

set -e  # Exit on error

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║           Mini Instagram - Quick Setup Script                  ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Change to backend directory
cd "$(dirname "$0")/SMP_backend"

# Check for g++
echo -n "Checking for g++ compiler... "
if command -v g++ &> /dev/null; then
    echo -e "${GREEN}✓${NC}"
    g++ --version | head -1
else
    echo -e "${RED}✗${NC}"
    echo "Error: g++ compiler not found. Please install g++."
    exit 1
fi

echo ""

# Check C++ version support
echo -n "Checking C++17 support... "
TEMP_FILE=$(mktemp)
echo "int main(){}" | g++ -std=c++17 -x c++ - -o "$TEMP_FILE" 2>/dev/null
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓${NC}"
    rm -f "$TEMP_FILE"
else
    echo -e "${RED}✗${NC}"
    rm -f "$TEMP_FILE"
    echo "Error: C++17 support required but not available."
    exit 1
fi

echo ""

# Create necessary directories
echo "Creating necessary directories..."
mkdir -p build
mkdir -p data/logs
echo -e "${GREEN}✓${NC} Directories created"

echo ""

# Build the project
echo "Building Mini Instagram..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

if [ -f "Makefile" ]; then
    echo "Using Makefile build..."
    make release
else
    echo "Using manual build..."
    g++ -std=c++17 -O2 -Wall -Wextra \
        src/main.cpp \
        src/content/feed.cpp \
        src/content/post.cpp \
        src/content/recommendation.cpp \
        src/core/followerList.cpp \
        src/core/relationGraph.cpp \
        src/core/user.cpp \
        src/core/status.cpp \
        src/interaction/message.cpp \
        src/interaction/friendRequest.cpp \
        src/system/notification.cpp \
        src/system/search.cpp \
        src/system/systemManager.cpp \
        src/utils/fileIO.cpp \
        src/utils/helpers.cpp \
        src/utils/validation.cpp \
        -I include \
        -o build/Mini_Instagram
fi

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}✓ Build completed successfully!${NC}"
    echo -e "${GREEN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo "To run the application:"
    echo -e "  ${YELLOW}./build/Mini_Instagram${NC}"
    echo ""
    echo "Or simply run:"
    echo -e "  ${YELLOW}./setup.sh run${NC}"
    echo ""
else
    echo ""
    echo -e "${RED}✗ Build failed!${NC}"
    echo "Please check the error messages above."
    exit 1
fi

# If 'run' argument is provided, run the application
if [ "$1" = "run" ]; then
    echo ""
    echo "Launching Mini Instagram..."
    echo ""
    ./build/Mini_Instagram
fi
