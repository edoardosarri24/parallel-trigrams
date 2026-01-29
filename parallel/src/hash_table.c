#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "hash_table.h"
#include "my_utils.h"

#define HASH_PRIME_BASE 257
#define ARENA_BLOCK_SIZE (16 * 1024 * 1024) // 16MB blocks.

static uint_fast32_t hash_function(const char *gram, const int buckets_size) {
    uint_fast32_t hash_value = 0;
    while (*gram) {
        unsigned char c = (unsigned char)*gram;
        hash_value = (hash_value * HASH_PRIME_BASE + c) % buckets_size; // thanks to the distributive property of the module operation
        gram++;
    }
    return hash_value;
}

HashTable *create_hash_table(int buckets_size) {
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    check_initialization(table, "Failed to allocate memory for hash table");
    table->buckets_size = buckets_size;
    table->buckets = (Node **)calloc(buckets_size, sizeof(Node *));
    check_initialization_eventually_free(table->buckets, table, "Failed to allocate memory for hash table buckets");
    table->mem_arena = create_arena(ARENA_BLOCK_SIZE);
    return table;
}

void add_gram(HashTable *table, const char *gram, size_t gram_len) {
    uint_fast32_t index = hash_function(gram, table->buckets_size);
    // if the gram is present, increment its counter.
    Node *current_node = table->buckets[index];
    while (current_node) {
        if (strcmp(current_node->gram, gram) == 0) {
            current_node->counter++;
            return;
        }
        current_node = current_node->next;
    }
    // otherwise define a new block in the arena.
    size_t requested_size = sizeof(Node) + gram_len + 1; // the size of the block.
    Node *new_node = (Node *)arena_alloc(table->mem_arena, requested_size);
    new_node->gram = (char *)((uint8_t *)new_node + sizeof(Node)); // this point at the address of data (see ArenaBlock struct).
    strcpy(new_node->gram, gram);
    new_node->counter = 1;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

void add_gram_to_bucket(HashTable *table, int bucket_index, const char *gram, int count, Arena *alloc_arena) {
    // travrse the global chain to find if it exists.
    Node *current_node = table->buckets[bucket_index];
    // if exists, sum the counter of new n-gram to the old counter.
    while (current_node) {
        if (strcmp(current_node->gram, gram) == 0) {
            current_node->counter += count;
            return;
        }
        // else define a new node.
        current_node = current_node->next;
    }
    // if not found, create a new node in global chain.
    size_t gram_len = strlen(gram);
    size_t requested_size = sizeof(Node) + gram_len + 1;
    Node *new_node = (Node *)arena_alloc(alloc_arena, requested_size);
    new_node->gram = (char *)((uint8_t *)new_node + sizeof(Node));
    strcpy(new_node->gram, gram);
    new_node->counter = count;
    new_node->next = table->buckets[bucket_index];
    table->buckets[bucket_index] = new_node;
}

void free_hash_table(HashTable *table) {
    check_ptr(table, "Hash table pointer is nullptr");
    // Free all block of the arena. then free the table buckets and the table.
    arena_free(table->mem_arena);
    free(table->buckets);
    free(table);
}