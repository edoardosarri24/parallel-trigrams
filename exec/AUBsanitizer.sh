#!/bin/bash

./exec/download_data.sh
echo "building..."
rm -rf build
cmake -S . -B build -DENABLE_AUBSAN=ON -DENABLE_PROFILING=OFF -DENABLE_MSAN=OFF
cmake --build build
echo "executing..."
./build/sequential/sequential_trigrams