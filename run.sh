#!/bin/bash

set -e # the script stops immediately if something fails

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# CMake configuration only happens the first time.
if [ ! -f "CMakeCache.txt" ]; then
    cmake ..
fi

cmake --build .

./planner