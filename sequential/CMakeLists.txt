#project
cmake_minimum_required(VERSION 3.10)
project(sequential_trigrams C)

# main
add_executable(sequential_trigrams)
target_sources(sequential_trigrams PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
${CMAKE_CURRENT_SOURCE_DIR}/src/hash_table.c)

# iperparameters
set(N_GRAM_SIZE 3 CACHE STRING "The size of the n-grams to process")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

# directory
target_include_directories(sequential_trigrams PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

# compile
target_compile_options(sequential_trigrams PRIVATE -Wall -Wextra)
target_compile_features(sequential_trigrams PRIVATE c_std_99)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build")
endif()