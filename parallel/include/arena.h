#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct Arena Arena;

// creates a new memory arena.
Arena *create_arena(size_t default_block_size);

// allocates memory from the arena and returns a pointer to the allocated memory.
void *arena_alloc(Arena *arena, size_t requested_size);

// transfer ownership of blocks from src to dest. src becomes empty.
void arena_connect(Arena *dest, Arena *src);

// Frees all memory associated with the arena.
void arena_free(Arena *arena);

#endif // ARENA_H