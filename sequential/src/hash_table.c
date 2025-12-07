#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "hash_table.h"
#include "config.h"

#define P_CHAR 37
#define M_CHAR 16777259
#define P_GRAM 101

typedef struct Node {
    char *gram;
    int counter;
    struct Node *next;
} Node;

struct HashTable {
    Node **buckets;
    size_t buckets_size; // number of index
};

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

static uint_fast32_t hash_word(const char *str, const size_t len) {
    uint_fast32_t hash_value = 0;
    for (size_t i = 0; i < len; i++) {
        hash_value = (hash_value * P_CHAR + (unsigned char)str[i]) % M_CHAR;
    }
    return hash_value;
}

static uint_fast32_t hash_function(const char *gram, size_t buckets_size) {
    uint_fast32_t hash_value = 0;
    const char *ptr = gram;
    // compute the hash_value for n-gram
    for (int i=0; i < N_GRAM_SIZE; i++) {
        const char *word_start = ptr;
        size_t len = 0;
        while (*ptr && *ptr != ' ') {
            ptr++;
            len++;
        }
        // compute the single word hash-value
        uint_fast32_t hash_value_word = hash_word(word_start, len);
        hash_value = (hash_value * P_GRAM + hash_value_word) % buckets_size;
        if (*ptr == ' ') ptr++;
    }
    return hash_value;
}

void add_gram(HashTable *table, const char *gram) {
    uint_fast32_t index = hash_function(gram, table->buckets_size);
    // if the gram is present, increment its counter.
    Node *current_node = table->buckets[index];
    while (current_node != NULL) {
        if (strcmp(current_node->gram, gram) == 0) {
            current_node->counter++;
            return;
        }
        current_node = current_node->next;
    }
    // althought create new node and insert it in the head of its related open chain.
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        perror("memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_node->gram = strdup(gram);
    if (!new_node->gram) {
        perror("memory allocation failed");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    new_node->counter = 1;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

void free_hash_table(HashTable *table) {
    if (table == NULL) return;

    for (size_t i=0; i < table->buckets_size; i++) {
        Node *current_node = table->buckets[i];
        while (current_node != NULL) {
            // the next node will be free in the next iteration
            Node *temp = current_node;
            current_node = current_node->next;
            free(temp->gram);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}