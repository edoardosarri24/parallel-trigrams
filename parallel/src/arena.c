#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "arena.h"
#include "my_utils.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct ArenaBlock {
    struct ArenaBlock *next; // next block. Usefull for free.
    size_t size; // byte size of the block.
    size_t used; // byte used in the block. We can see like a cursor for allocations.
    uint8_t data[]; // point to first byte after the struct. Its dimensione depends on requested_size during the allocation.
} ArenaBlock;

struct Arena {
    ArenaBlock *head; // first address to the start of the first block. Usefull for free.
    ArenaBlock *current; // current block for new allocations.
    size_t default_block_size;
};

static ArenaBlock *arena_new_block(size_t size) {
    // Allocate struct + data
    ArenaBlock *block = (ArenaBlock *)malloc(sizeof(ArenaBlock) + size);
    check_initialization(block, "Failed to allocate arena block");
    block->next = nullptr;
    block->size = size;
    block->used = 0;
    return block;
}

Arena *create_arena(size_t default_block_size) {
    Arena *arena = (Arena *)malloc(sizeof(Arena));
    check_initialization(arena, "Failed to allocate arena struct");
    arena->head = nullptr; // this function create only the struct, no blocks yet (untill arena_alloc).
    arena->current = nullptr;
    arena->default_block_size = default_block_size;
    return arena;
}

void *arena_alloc(Arena *arena, size_t requested_size) {
    // round up the request size to the nearest multiple of alignment (8 bytes in 64bit systems).
    size_t alignment = sizeof(void*);
    requested_size = (requested_size + alignment - 1) & ~(alignment - 1);
    // return the next block if there is enough space.
    if (arena->current && (arena->current->used+requested_size <= arena->current->size)) {
        void *next_block_ptr = arena->current->data + arena->current->used;
        arena->current->used += requested_size;
        return next_block_ptr;
    }
    // otherwise, allocate a new block.
    size_t next_block_size = MAX(requested_size, arena->default_block_size);
    ArenaBlock *new_block = arena_new_block(next_block_size);
    // insert the block as the first of the list.
    new_block->next = arena->head;
    arena->head = new_block;
    arena->current = new_block;
    void *ptr = arena->current->data + arena->current->used;
    arena->current->used += requested_size;
    return ptr;
}

void arena_connect(Arena *dest, Arena *src) {
    if (!dest || !src || !src->head) return;
    // find the final block in src.
    ArenaBlock *tail = src->head;
    while (tail->next) {
        tail = tail->next;
    }
    // attach dest to src
    tail->next = dest->head;
    // the head began the head of src
    dest->head = src->head;
    // clear src to avoid double free: arena_free and free_hash_table.
    src->head = nullptr;
    src->current = nullptr;
}

void arena_free(Arena *arena) {
    if (!arena) return;
    // free all blocks.
    ArenaBlock *block = arena->head;
    while (block) {
        ArenaBlock *next = block->next;
        free(block);
        block = next;
    }
    // free the arena struct.
    free(arena);
}