#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "arena.h"

typedef struct Node {
    char *gram;
    int counter;
    struct Node *next;
} Node;

typedef struct HashTable {
    Node **buckets;
    int buckets_size; // index number of the table.
    Arena *mem_arena;
} HashTable;

HashTable *create_hash_table(int buckets_size);
void add_gram(HashTable *table, const char *gram, size_t gram_len);
void add_gram_to_bucket(HashTable *table, int bucket_index, const char *gram, int count, Arena *alloc_arena);
void free_hash_table(HashTable *table);

#endif // HASH_TABLE_H