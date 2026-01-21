#include <stdio.h>
#include <stdlib.h>
#include "statistics.h"
#include "hash_table.h"
#include "my_utils.h"

static int compare_nodes(const void *a, const void *b) {
    const Node *nodeA = *(const Node **)a;
    const Node *nodeB = *(const Node **)b;
    // Descending order
    return (nodeB->counter - nodeA->counter);
}

static int find_element_with_min_frequency(Node **top_ngrams) {
    int min_idx = 0;
    for (int j=1; j < TOP_K; ++j) {
        if (top_ngrams[j]->counter < top_ngrams[min_idx]->counter)
            min_idx=j;
    }
    return min_idx;
}

TextStatistics calculate_text_statistics(HashTable *hashTable) {
    check_ptr(hashTable, "calculate_text_statistics in statistics.c received nullptr hashTable");
    TextStatistics stats = {0};
    // Travers the hashtable: increment the counter and find top k ngrams.
    for (int i=0; i < hashTable->buckets_size; ++i) {
        Node *current = hashTable->buckets[i];
        while (current) {
            stats.unique_ngrams++;
            // check if top_ngrams is not full
            if (stats.top_filled < TOP_K) {
                stats.top_ngrams[stats.top_filled] = current;
                stats.top_filled++;
            } else {
                // Find the index of element with the minimum frequency in top_ngrams.
                int min_idx = find_element_with_min_frequency(stats.top_ngrams);
                // If current node has higher frequency than the minimum, replace it.
                if (current->counter > stats.top_ngrams[min_idx]->counter) {
                    stats.top_ngrams[min_idx] = current;
                }
            }
            current = current->next;
        }
    }
    // sort top_ngrams if there is at least one ngram.
    if (stats.unique_ngrams > 0) {
        qsort(stats.top_ngrams, stats.top_filled, sizeof(Node *), compare_nodes);
    }
    return stats;
}

HashTableStatistics calculate_hashtable_statistics(HashTable *hashTable) {
    check_ptr(hashTable, "calculate_hashtable_statistics received nullptr hashTable");
    HashTableStatistics stats = {0};
    stats.buckets_size = hashTable->buckets_size;
    // traverse hashtable and collets statistics.
    for (int i=0; i < hashTable->buckets_size; ++i) {
        Node *current = hashTable->buckets[i];
        if (current) {
            stats.busy_buckets++;
            size_t current_chain_len = 0;
            while (current) {
                current_chain_len++;
                current = current->next;
            }
            stats.total_elements += current_chain_len;
            if (current_chain_len > stats.max_chain_len)
                stats.max_chain_len = current_chain_len;
        }
    }
    return stats;
}