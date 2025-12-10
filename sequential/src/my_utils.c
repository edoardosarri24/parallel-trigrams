#include <stdlib.h>
#include "my_utils.h"

char *next_ngram(FILE *src) {
    // variables.
    char words[N_GRAM_SIZE][256];
    char *ngram = (char *)malloc(N_GRAM_SIZE * 256 * sizeof(char));
    long start_position = ftell(src);
    // save the words of the next ngram.
    for (int i=0; i<N_GRAM_SIZE; i++) {
        if (fscanf(src, "%255s", words[i]) != 1)
            return NULL;
    }
    // compose the next ngrams.
    int offset = 0;
    for (int i = 0; i < N_GRAM_SIZE; i++) {
        offset += sprintf(ngram + offset, "%s%s", words[i], (i == N_GRAM_SIZE - 1) ? "" : " ");
    }
    // come back to the original file poitner position.
    fseek(src, start_position, SEEK_SET);
    return ngram;
}