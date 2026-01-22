#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "main_functions.h"
#include "hash_table.h"

#define HASH_TABLE_DIMENSION 10000019
#define MAX_WORD_LEN 256

static bool get_next_word(const char **data_cursor, const char *data_end, char *next_word, size_t *word_len) {
    const char *current_data_cursor = *data_cursor; // double pointer allows updating the caller's cursor
    // skip non-alphanumeric characters. This bring us at the start of the next word.
    while (current_data_cursor < data_end && !isalnum((unsigned char)*current_data_cursor))
        current_data_cursor++;
    // EOF check.
    if (current_data_cursor >= data_end) {
        *data_cursor = current_data_cursor;
        return false;
    }
    // read the next word.
    size_t i = 0;
    while (current_data_cursor < data_end && isalnum((unsigned char)*current_data_cursor)) {
        if (i < MAX_WORD_LEN-1)
            next_word[i++] = tolower((unsigned char)*current_data_cursor);
        current_data_cursor++;
    }
    next_word[i] = '\0';
    if (word_len) *word_len = i; // return also the length of the word.
    // return.
    *data_cursor = current_data_cursor;
    return true;
}

const char* map_file(const char *input_filepath, size_t *input_size) {
    // open file
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
    // check the file size
    *input_size = sb.st_size;
    if (sb.st_size == 0) {
        close(fd);
        return nullptr;
    }
    // mapping the file.
    const char *input_data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (input_data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    // the data will be accessed sequentially. It is an optimization for the kernel.
    if (madvise((void *)input_data, sb.st_size, MADV_SEQUENTIAL) == -1)
        perror("Warning: madvise failed");
    // return
    close(fd);
    return input_data;
}

HashTable* populate_hashtable(const char *input_data, size_t input_size) {
    // initializing hash table if the input file is ok.
    HashTable *hashTable = create_hash_table(HASH_TABLE_DIMENSION);
    if (input_size == 0 || input_data == nullptr)
        return hashTable;
    // variables.
    char words[N_GRAM_SIZE][MAX_WORD_LEN]; // circular buffer of the n-gram.
    size_t lengths[N_GRAM_SIZE]; // circular buffer for word lengths.
    const char *data_cursor = input_data;
    const char *data_end = input_data + input_size;
    // fill the N_GRAM_SIZE-1 initial words in the circular buffer.
    for (int i=0; i<N_GRAM_SIZE-1; i++) {
        // if there aren't N_GRAM_SIZE word in the input file, return an empty hash table.
        if (!get_next_word(&data_cursor, data_end, words[i], &lengths[i])) // passing the address we can update the cursor
            return hashTable;
    }
    // process the input file word by word.
    int head = 0; // points to the start of current n-gram.
    char current_ngram_string[N_GRAM_SIZE * MAX_WORD_LEN]; // the start of the new k-gram.
    while (get_next_word(&data_cursor, data_end, words[(head + N_GRAM_SIZE-1) % N_GRAM_SIZE], &lengths[(head + N_GRAM_SIZE-1) % N_GRAM_SIZE])) {
        // Construct the n-gram string from the current window
        char *dest = current_ngram_string; // mobile cursor of the new k-gram.
        for (int i=0; i<N_GRAM_SIZE; i++) {
            int idx = (head + i) % N_GRAM_SIZE;
            size_t len = lengths[idx];
            memcpy(dest, words[idx], len);
            dest += len;
            if (i < N_GRAM_SIZE - 1) {
                *dest = ' ';
                dest++;
            }
        }
        *dest = '\0';
        //handle the current n-gram.
        add_gram(hashTable, current_ngram_string);
        // the head becomes the next word.
        head = (head+1) % N_GRAM_SIZE;
    }
    // return
    return hashTable;
}