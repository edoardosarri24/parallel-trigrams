cmake_minimum_required(VERSION 3.21)
project(n-grams C)

# set default build type to Release if not specified.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# standard C configuration.
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# global warnings
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wno-unused-parameter)

# performance flags.

include(CheckIPOSupported) # optimizing in link pahse.
check_ipo_supported(RESULT lto_supported OUTPUT error)

if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
    add_compile_options(-march=native -mtune=native)

    if(lto_supported)
        message(STATUS "LTO (Link Time Optimization) is enabled.")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${error}")
    endif()

    add_compile_options(-O3 -funroll-loops)
endif()

# sanitizers.
option(ENABLE_AUBSAN "Enable address and undefined sanitizers" OFF)
option(ENABLE_MSAN "Enable memory sanitizer" OFF)

if(ENABLE_AUBSAN AND ENABLE_MSAN)
    message(FATAL_ERROR "Cannot enable both ASan and MSan at the same time.")
endif()

if(ENABLE_AUBSAN)
    message(STATUS "Sanitizers: Address and Undefined behavior enabled.")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address,undefined)
elseif(ENABLE_MSAN)
    message(STATUS "Sanitizers: Memory enabled.")
    add_compile_options(-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=memory)
endif()

# OpenMP.
if(APPLE)
    if(EXISTS "/opt/homebrew/opt/libomp")
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    elseif(EXISTS "/usr/local/opt/libomp")
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    endif()
endif()
find_package(OpenMP REQUIRED)

# profiling (GPerftools).
option(ENABLE_PROFILING "Link against gperftools for CPU profiling" OFF) # Default OFF to avoid linking issues unless requested
if(ENABLE_PROFILING)
    find_library(GPERFTOOLS_PROFILER profiler)
    if(GPERFTOOLS_PROFILER)
        message(STATUS "Profiling enabled: Linking against gperftools.")
        add_link_options(${GPERFTOOLS_PROFILER})
        # Profiling needs frame pointers and symbols
        add_compile_options(-fno-omit-frame-pointer -g)
    else()
        message(WARNING "GPerftools profiler library not found. Install it to enable profiling.")
    endif()
endif()

# hyperparameters.
set(N_GRAM_SIZE 3 CACHE STRING "Size of the n-gram")
set(TOP_K 3 CACHE STRING "Number of top n-grams to display")

# subprojects
add_subdirectory(sequential)
add_subdirectory(parallel)