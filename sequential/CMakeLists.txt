#project
cmake_minimum_required(VERSION 3.10)
project(sequential_trigrams C)

# variables
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build")
endif()

# main
add_executable(sequential_trigrams)
target_sources(sequential_trigrams PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hash_table.c)
target_include_directories(sequential_trigrams PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# compile options
target_compile_options(sequential_trigrams PRIVATE -Wall -Wextra)

# compile features
target_compile_features(sequential_trigrams PRIVATE c_std_99)