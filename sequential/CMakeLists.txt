#project
cmake_minimum_required(VERSION 3.10)
project(sequential_trigrams C)

# main
add_executable(sequential_trigrams)
target_sources(sequential_trigrams PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hash_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pre_processing.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/my_utils.c)

# iperparameters
set(N_GRAM_SIZE 3)
set(STRIDE 1)

# global macros
target_compile_definitions(sequential_trigrams PRIVATE
    DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    N_GRAM_SIZE=${N_GRAM_SIZE}
    STRIDE=${STRIDE})

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