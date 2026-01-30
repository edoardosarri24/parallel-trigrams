#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include "main_functions.h"
#include "hash_table.h"
#include "parameters.h"
#include "my_utils.h"

// Modified to return the start position of the word found
static bool get_next_word(const char **data_cursor, const char *data_end, char *next_word, size_t *word_len, const char **word_start_out) {
    const char *current_data_cursor = *data_cursor;
    // skip non-alphanumeric characters. This bring us at the start of the next word.
    while (current_data_cursor < data_end && !isalnum((unsigned char)*current_data_cursor))
        current_data_cursor++;
    // EOF check
    if (current_data_cursor >= data_end) {
        *data_cursor = current_data_cursor;
        return false;
    }
    // determine word start. Usefull for the paralell part
    if (word_start_out) *word_start_out = current_data_cursor;
    // read the next word.
    size_t i = 0;
    while (current_data_cursor < data_end && isalnum((unsigned char)*current_data_cursor)) {
        if (i < MAX_WORD_LEN-1)
            next_word[i++] = (char)tolower((unsigned char)*current_data_cursor);
        current_data_cursor++;
    }
    next_word[i] = '\0';
    // return.
    if (word_len) *word_len = i; // return also the length of the word.
    *data_cursor = current_data_cursor;
    return true;
}

const char* map_file(const char *input_filepath, size_t *input_size) {
    int fd = open(input_filepath, O_RDONLY);
    if (fd == -1) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        close(fd);
        exit(EXIT_FAILURE);
    }
    *input_size = (size_t)sb.st_size;
    if (sb.st_size == 0) {
        close(fd);
        return NULL;
    }
    const char *input_data = mmap(NULL, (size_t)sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (input_data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (madvise((void *)input_data, (size_t)sb.st_size, MADV_SEQUENTIAL) == -1)
        perror("Warning: madvise failed");
    close(fd);
    return input_data;
}

static void populate_local_table(HashTable *my_table, const char *data_cursor, const char *my_end, const char *end) {
    // variables.
    char words[N_GRAM_SIZE][MAX_WORD_LEN];
    size_t lengths[N_GRAM_SIZE];
    const char *word_starts[N_GRAM_SIZE]; // Circular buffer for word start positions
    // fill the N_GRAM_SIZE-1 initial words in the circular buffer.
    for (int i=0; i < N_GRAM_SIZE-1; i++) {
        if (!get_next_word(&data_cursor, end, words[i], &lengths[i], &word_starts[i])) {
            return;
        }
        // a k-gram is in my zone if its first word in my zone.
        if (i==0 && my_end < end && word_starts[0] >= my_end) { // the last thread can not enter in the other thread zone.
            return;
        }
    }
    int head = 0;
    char current_ngram_string[N_GRAM_SIZE * MAX_WORD_LEN];  // the start of the new k-gram.
    // main loop.
    while (get_next_word(&data_cursor, end, words[(head + N_GRAM_SIZE - 1) % N_GRAM_SIZE],
            &lengths[(head + N_GRAM_SIZE - 1) % N_GRAM_SIZE],
            &word_starts[(head + N_GRAM_SIZE - 1) % N_GRAM_SIZE])) {
        //check if the current k-gram belongs to this thread, i.e., if the the first word is within [my_start, my_end).
        if (my_end < end && word_starts[head] >= my_end)
            break;

        // construct the n-gram string from the current window.
        char *dest = current_ngram_string;
        for (int i = 0; i < N_GRAM_SIZE; i++) {
            int idx = (head + i) % N_GRAM_SIZE;
            size_t len = lengths[idx];
            memcpy(dest, words[idx], len);
            dest += len;
            if (i < N_GRAM_SIZE-1) {
                *dest = ' ';
                dest++;
            }
        }
        *dest = '\0';
        // handle the current n-gram.
        size_t ngram_len = (size_t)(dest - current_ngram_string);
        add_gram(my_table, current_ngram_string, ngram_len);
        head = (head + 1) % N_GRAM_SIZE;
    }
}

HashTable* populate_hashtable(const char *start, const char *end) {
    // initializing hash table and continue if the input file is ok.
    HashTable *global_table = create_hash_table(HASH_TABLE_DIMENSION);
    if (start == NULL || end == NULL || start >= end)
        return global_table;
    // allocation of local table accessible out of threads.
    int max_threads = omp_get_max_threads(); // avoid segmentation falut on the local hashtable.
    HashTable **local_tables = (HashTable **)calloc((size_t)max_threads, sizeof(HashTable*));
    check_initialization(local_tables, "Failed to allocate local_tables array");
    #pragma omp parallel default(none) shared(local_tables, global_table) firstprivate(start, end)
    {
        // chunking and threads bounds.
        int tid = omp_get_thread_num();
        int n_threads = omp_get_num_threads();
        size_t total_len = (size_t)(end - start);
        size_t chunk_size = total_len / (size_t)n_threads;
        // calculate the thread bounds.
        const char *my_start = start + (size_t)tid * chunk_size;
        const char *my_end = (tid == n_threads - 1) ? end : start + ((size_t)tid + 1) * chunk_size;
        if (tid > 0) { // the main thread is already allign with the correct (first) word.
            // If the character before my_start is alphanumeric, we are inside a word or at its end.
            if (my_start > start && isalnum((unsigned char)*(my_start - 1))) { // if the first character isn't the first character...
                while (my_start < end && isalnum((unsigned char)*my_start)) // skip at the start of the next word.
                    my_start++;
            }
        }
        // initialize local hash table.
        HashTable *my_table = create_hash_table(HASH_TABLE_DIMENSION);
        local_tables[tid] = my_table;
        // populate local hash table.
        populate_local_table(my_table, my_start, my_end, end);
        #pragma omp barrier // wait the population of all local hastable.
        // merge phase
        int bucket_chunk_size = (64 / sizeof(Node*)) * 16; // hadles the false sharing and the overhead.
        #pragma omp for schedule(dynamic, bucket_chunk_size) // dinamyc hadles the sparsity of the hash table
        for (int i=0; i < HASH_TABLE_DIMENSION; i++) {
            for (int t=0; t < n_threads; t++) {
                Node *local_node = local_tables[t]->buckets[i];
                while (local_node) { // iterate while there is another element in i-th chain in t-th table.
                    Node *next_local_node = local_node->next;
                    // search for this node in i-th global hashtable bucket.
                    Node *global_node = global_table->buckets[i];
                    bool found = false;
                    while (global_node) {
                        if (strcmp(global_node->gram, local_node->gram) == 0) {
                            global_node->counter += local_node->counter;
                            found = true;
                            break;
                        }
                        global_node = global_node->next;
                    }
                    // if not found, move this node to the global table.
                    if (!found) {
                        local_node->next = global_table->buckets[i];
                        global_table->buckets[i] = local_node;
                    }
                    local_node = next_local_node;
                }
            }
        }
        // safely merge the thread-local arena into the global one.
        #pragma omp critical
        {
            arena_connect(global_table->mem_arena, my_table->mem_arena);
        }
        // free local structs only, data blocks are now in global arena.
        arena_free(my_table->mem_arena);
        free(my_table->buckets);
        free(my_table);
    }
    free(local_tables);
    return global_table;
}
