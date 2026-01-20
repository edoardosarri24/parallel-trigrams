#!/bin/bash

./exec/download_data.sh
cmake -S . -B build -DENABLE_SANITIZER=OFF -DENABLE_PROFILING=ON
cmake --build build
CPUPROFILE=whole.prof ./build/sequential/sequential_trigrams
/Users/edoardosarri/.asdf/installs/golang/1.25.6/bin/pprof -top ./build/sequential/sequential_trigrams whole.prof > sequential/result_profiling/profile.txt
/Users/edoardosarri/.asdf/installs/golang/1.25.6/bin/pprof -pdf ./build/sequential/sequential_trigrams whole.prof > sequential/result_profiling/profile.pdf
rm whole.prof