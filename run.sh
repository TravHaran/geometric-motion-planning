#!/bin/bash

set -e # the script stops immediately if something fails

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-release"

if [ -z "$1" ]; then
    echo "Usage: ./run.sh <demo>"
    echo "Example: ./run.sh demo_1a"
    exit 1
fi

DEMO_TARGET="$1"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --target "$DEMO_TARGET"

cd "$BUILD_DIR"
./"$DEMO_TARGET"
