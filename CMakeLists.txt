cmake_minimum_required(VERSION 3.21)
project(trigrams C)

# main configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build")
endif()
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra) # fault and styling flags.

# sanitizers
option(ENABLE_SANITIZER "Enable address and undefined sanitizers" OFF)
if(ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer) # the last is for clear messages
    add_link_options(-fsanitize=address,undefined)
endif()

# OpenMP
find_package(OpenMP REQUIRED)

# GPerftools Profiling
option(ENABLE_PROFILING "Link against gperftools for CPU profiling" ON)
if(ENABLE_PROFILING)
    find_library(GPERFTOOLS_PROFILER profiler)
    if(GPERFTOOLS_PROFILER)
        add_link_options(${GPERFTOOLS_PROFILER})
        add_compile_options(-O3 -g -fno-omit-frame-pointer)
    else()
        message(WARNING "GPerftools profiler library not found. Install it (e.g., 'brew install gperftools') to enable profiling.")
    endif()
endif()

# hyperparameters
set(N_GRAM_SIZE 3 CACHE STRING "Size of the n-gram")
set(STRIDE 1 CACHE STRING "Stride for the sliding window")
set(TOP_K 10 CACHE STRING "Number of top n-grams to display")

# projects
add_subdirectory(sequential)