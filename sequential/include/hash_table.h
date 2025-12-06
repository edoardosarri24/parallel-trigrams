#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>

typedef struct Node {
    char *gram;
    int counter;
    struct Node *next;
} Node;

typedef struct {
    Node **buckets;
    size_t buckets_size; // number of index
} HashTable;

HashTable *create_hash_table(size_t buckets_size);

#endif // HASH_TABLE_H