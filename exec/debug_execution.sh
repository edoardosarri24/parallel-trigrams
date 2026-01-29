#!/bin/bash

if [ -z "$1" ]; then
    echo "Error: Missing argument. Usage: $0 [seq|par]"
    exit 1
fi

MODE=$1

if [[ "$MODE" != "seq" && "$MODE" != "par" ]]; then
    echo "Error: Invalid argument '$MODE'. Usage: $0 [seq|par]"
    exit 1
fi

echo "building..."
rm -rf build
cmake -S . -B build -DENABLE_AUBSAN=OFF -DENABLE_PROFILING=OFF -DENABLE_MSAN=OFF
cmake --build build

echo "executing ($MODE)..."
if [ "$MODE" == "par" ]; then
    ./build/parallel/parallel_trigrams
else
    ./build/sequential/sequential_trigrams
fi