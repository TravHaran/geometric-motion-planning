#!/bin/bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

if [ ! -f "CMakeCache.txt" ]; then
    cmake ..
fi

cmake --build .

./geometry_tests
./graph_tests
./dijkstra_tests
./astar_tests