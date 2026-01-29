cmake_minimum_required(VERSION 3.21)
project(trigrams C)

# main configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native)
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra) # fault and styling flags.

# sanitizers ENABLE_AUBSAN
option(ENABLE_AUBSAN "Enable address and undefined sanitizers" OFF)
option(ENABLE_MSAN "Enable memory sanitizer" OFF)
if(ENABLE_AUBSAN AND ENABLE_MSAN)
    message(FATAL_ERROR "Cannot enable both AddressSanitizer (ENABLE_AUBSAN) and MemorySanitizer (ENABLE_MSAN) at the same time.")
endif()
if(ENABLE_AUBSAN)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer) # the last is for clear messages
    add_link_options(-fsanitize=address,undefined)
elseif(ENABLE_MSAN)
    add_compile_options(-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer)
    add_link_options(-fsanitize=memory)
endif()

# OpenMP
if(APPLE)
    if(EXISTS "/opt/homebrew/opt/libomp")
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    elseif(EXISTS "/usr/local/opt/libomp")
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    endif()
endif()
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
set(TOP_K 10 CACHE STRING "Number of top n-grams to display")

# projects
add_subdirectory(sequential)
add_subdirectory(parallel)