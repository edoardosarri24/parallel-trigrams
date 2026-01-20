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

void print_text_statistics(HashTable *hashTable) {
    check_ptr(hashTable, "print_text_statistics in statistics.c received nullptr hashTable");
    // variables.
    size_t unique_ngrams = 0;
    Node *top_ngrams[TOP_K] = {nullptr};
    int filled = 0;
    // Travers the hashtable: increment the counter and fnd top k ngrams.
    for (int i=0; i < hashTable->buckets_size; ++i) {
        Node *current = hashTable->buckets[i];
        while (current) {
            unique_ngrams++;
            // check if top_ngrams is not full
            if (filled < TOP_K) {
                top_ngrams[filled] = current;
                filled++;
            } else {
                // Find the index of element with the minimum frequency in top_ngrams.
                int min_idx = find_element_with_min_frequency(top_ngrams);
                // If current node has higher frequency than the minimum, replace it.
                if (current->counter > top_ngrams[min_idx]->counter) {
                    top_ngrams[min_idx] = current;
                }
            }
            current = current->next;
        }
    }
    if (unique_ngrams == 0) {
        printf("No n-grams found.\n");
        return;
    }
    // sort top_ngrams and print statistics.
    qsort(top_ngrams, filled, sizeof(Node *), compare_nodes);
    printf("Total unique n-grams: %zu\n", unique_ngrams);
    printf("Top %d n-grams:\n", TOP_K);
    for (int i = 0; i < filled; ++i) {
        printf("%d - '%s': %d\n", i + 1, top_ngrams[i]->gram, top_ngrams[i]->counter);
    }
}

void print_hashtable_statistics(HashTable *hashTable) {
    check_ptr(hashTable, "print_hashtable_statistics received nullptr hashTable");
    // variables.
    size_t total_elements = 0;
    size_t busy_buckets = 0;
    size_t max_chain_len = 0;
    // traverse hashtable and collets statistics.
    for (int i=0; i < hashTable->buckets_size; ++i) {
        Node *current = hashTable->buckets[i];
        if (current) {
            busy_buckets++;
            size_t current_chain_len = 0;
            while (current) {
                current_chain_len++;
                current = current->next;
            }
            total_elements += current_chain_len;
            if (current_chain_len > max_chain_len)
                max_chain_len = current_chain_len;
        }
    }
    // print statistics.
    printf("-Total buckets: %d\n", hashTable->buckets_size);
    printf("-Busy buckets: %zu\n", busy_buckets);
    printf("-Total elements: %zu\n", total_elements);
    printf("-Max chain length: %zu\n", max_chain_len);
    printf("-Avg chain length: %.2f\n",  busy_buckets>0 ? ((double)total_elements / busy_buckets) : 0.0);
    printf("-Load Factor (elements/buckets): %.4f\n", (double)total_elements / hashTable->buckets_size);
    printf("-Fill Factor (busy/total): %.4f\n", (double)busy_buckets / hashTable->buckets_size);
}