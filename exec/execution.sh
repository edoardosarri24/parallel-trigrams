#!/bin/bash

./exec/download_data.sh
echo "building..."
rm -rf build
cmake -S . -B build -DENABLE_SANITIZER=OFF -DENABLE_PROFILING=OFF
cmake --build build
echo "executing..."
./build/sequential/sequential_trigrams