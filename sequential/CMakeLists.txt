project(sequential_trigrams C)
add_executable(sequential_trigrams)

# sources
target_sources(sequential_trigrams PRIVATE
    src/main.c
    src/hash_table.c
    src/main_functions.c
    src/statistics.c
    src/my_utils.c
)

# includes
target_include_directories(sequential_trigrams PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

# libraries
target_link_libraries(sequential_trigrams PRIVATE OpenMP::OpenMP_C)

# hyperparameters
target_compile_definitions(sequential_trigrams PRIVATE
    DATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"
    N_GRAM_SIZE=${N_GRAM_SIZE}
    STRIDE=${STRIDE}
    TOP_K=${TOP_K}
)