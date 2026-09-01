#!/bin/bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

cmake \
    -S "$PROJECT_ROOT" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build "$BUILD_DIR"

ctest \
    --test-dir "$BUILD_DIR" \
    --output-on-failure
