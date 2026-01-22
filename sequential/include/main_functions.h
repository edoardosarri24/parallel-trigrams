#ifndef MAIN_FUNCTIONS_H
#define MAIN_FUNCTIONS_H

#include <stdio.h>
#include "hash_table.h"

const char* map_file(const char *input_filepath, size_t *input_size);
HashTable* populate_hashtable(const char *input_data, size_t input_size);

#endif // MAIN_FUNCTIONS_H