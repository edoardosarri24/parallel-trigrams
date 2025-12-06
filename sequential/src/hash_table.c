#include <stdlib.h>
#include <stdio.h>
#include "hash_table.h"

HashTable *create_hash_table(size_t buckets_size) {
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    if (!table) {
        perror("memory allocation failed");
        exit(EXIT_FAILURE);
    }
    table->buckets_size = buckets_size;
    table->buckets = (Node **)calloc(buckets_size, sizeof(Node *));
    if (!table->buckets) {
        perror("memory allocation failed");
        free(table);
        exit(EXIT_FAILURE);
    }
    return table;
}