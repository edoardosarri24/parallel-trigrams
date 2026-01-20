#!/bin/bash

./exec/download_data.sh
cmake -S . -B build -DENABLE_SANITIZER=ON -DENABLE_PROFILING=OFF
cmake --build build
./build/sequential/sequential_trigrams