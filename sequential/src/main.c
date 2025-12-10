#include <stdlib.h>
#include "hash_table.h"
#include "pre_processing.h"
#include "my_utils.h"

#ifndef HASH_TABLE_DIMENSION
    #define HASH_TABLE_DIMENSION 150000
#endif

int main() {
    // pre-processing
    printf("Pre-processing the input file...\n");
    preprocess_file();

    // count the N-gram.
    FILE *src = fopen(DATA_DIR"/normalized_text.txt", "r");
    if (!src) {
        perror("Error opening input normalized file");
        exit(EXIT_FAILURE);
    }
    HashTable *hashTable = create_hash_table(HASH_TABLE_DIMENSION);
    char *gram;
    char temp_buffer[256];
    while ((gram = next_ngram(src)) != NULL) {
        printf("%s\n", gram);
        add_gram(hashTable,gram);
        for(int i=0; i<STRIDE; i++)
            fscanf(src, "%255s", temp_buffer);
    }
    fclose(src);

    // visualize result
    // TODO

    // close
    free_hash_table(hashTable);
    return EXIT_SUCCESS;
}