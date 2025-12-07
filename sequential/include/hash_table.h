#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

typedef struct HashTable HashTable;

HashTable *create_hash_table(size_t buckets_size);
void add_gram(HashTable *table, const char *gram);
void free_hash_table(HashTable *table);

#endif // HASH_TABLE_H