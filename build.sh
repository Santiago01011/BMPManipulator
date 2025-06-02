#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
CC=gcc
CFLAGS="-Wall -Wextra -std=c99 -g -O2 -Wno-sign-compare -Wno-missing-braces"
LDFLAGS="-lm"
EXECUTABLE="bmp_manipulator"
BINDIR="src/bin"


# Find all source files
SOURCES="$(find src -name '*.c')"

# SDL2, SDL2_ttf, SDL2_image development paths are set as environment variables, if not set, program won't compile
# export SDL2DIR="your/path/to/SDL2"
if [ -z "$SDL2DIR" ]; then
    echo -e "${RED}❌ SDL2DIR environment variable is not set. Please set it to your SDL2 development path.${NC}"
    exit 1
fi

# Include directories
INCLUDE_DIRS="-Isrc -Isrc/UI -Isrc/Utils -Isrc/BMPManipulator -I$SDL2DIR/include/SDL2"
LIB_DIRS="-L$SDL2DIR/lib"
LIBS="-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -luser32 -lgdi32 -lwinmm -ldxguid"

# Create bin directory if it doesn't exist
if [ ! -d "$BINDIR" ]; then
    echo -e "${YELLOW}Creating $BINDIR directory...${NC}"
    mkdir -p "$BINDIR"
fi

# Clean previous builds
if [ -f "$BINDIR/$EXECUTABLE" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -f "$BINDIR/$EXECUTABLE"
    rm -f *.o
fi

# Compile
WARNINGS=0
ERRORS=0

# Compile and capture output
COMPILE_OUTPUT=$(mktemp)
$CC $CFLAGS $INCLUDE_DIRS $LIB_DIRS $SOURCES -o "$BINDIR/$EXECUTABLE" $LIBS $LDFLAGS 2> "$COMPILE_OUTPUT"
COMPILE_STATUS=$?

# Count warnings and errors
if [ -s "$COMPILE_OUTPUT" ]; then
    WARNINGS=$(grep -c -i 'warning:' "$COMPILE_OUTPUT" || true)
    ERRORS=$(grep -c -i 'error:' "$COMPILE_OUTPUT" || true)
    cat "$COMPILE_OUTPUT"
fi
rm -f "$COMPILE_OUTPUT"

if [ $COMPILE_STATUS -eq 0 ]; then
    echo -e "${GREEN}✅ Compilation successful!${NC}"
    echo -e "${GREEN}Executable created: $BINDIR/$EXECUTABLE${NC}"
    echo -e "${YELLOW}Warnings: $WARNINGS${NC}"
    echo -e "${RED}Errors: $ERRORS${NC}"
    # Copy DLLs from SDL2 bin directory (as in Code::Blocks)
    cp "$SDL2DIR/bin/"*.dll "$BINDIR/" 2>/dev/null || true
    # Check if user wants to run the program
    if [ "$1" = "run" ] || [ "$1" = "-r" ]; then
        echo -e "${BLUE}Running the program...${NC}"
        if [ $# -gt 1 ]; then
            # Pass remaining arguments to the program
            shift
            cd src
            echo -e "${BLUE}Running with arguments: $@${NC}"
            ./bin/$EXECUTABLE "$@"
        else
            # Run with help or default
            echo -e "${YELLOW}No arguments provided. Showing usage help:${NC}"
            cd src
            ./bin/$EXECUTABLE --help 2>/dev/null || echo "Program compiled successfully but no help available."
        fi
    else
        echo -e "${YELLOW}To run the program, use:${NC}"
        echo "  ./$BINDIR/$EXECUTABLE [options]"
        echo "  Or run: ./build.sh run [options]"
    fi
else
    echo -e "${RED}❌ Compilation failed!${NC}"
    echo -e "${YELLOW}Warnings: $WARNINGS${NC}"
    echo -e "${RED}Errors: $ERRORS${NC}"
    exit 1
fi
