#ifndef STATISTICS_H
#define STATISTICS_H

#include "hash_table.h"
#include <stddef.h>

typedef struct TextStatistics {
    size_t unique_ngrams;
    Node *top_ngrams[TOP_K];
    int top_filled;
} TextStatistics;

typedef struct HashTableStatistics {
    int buckets_size;
    size_t busy_buckets;
    size_t total_elements;
    size_t max_chain_len;
} HashTableStatistics;

TextStatistics calculate_text_statistics(HashTable *hashTable);
HashTableStatistics calculate_hashtable_statistics(HashTable *hashTable);

#endif // STATISTICS_H