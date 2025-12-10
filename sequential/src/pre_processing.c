#include "pre_processing.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

void preprocess_file() {
    // open source and destination.
    const char *input_path = DATA_DIR"/input.txt";
    FILE *src = fopen(input_path, "r");
    if (!src) {
        fprintf(stderr, "Error opening input file. The file MUST be named %s and MUST be in %s directory.\n", "input.txt", DATA_DIR);
        exit(EXIT_FAILURE);
    }
    const char *output_path = DATA_DIR"/normalized_text.txt";
    FILE *dst = fopen(output_path, "w");
    if (!dst) {
        perror("Error opening output file");
        fclose(src);
        exit(EXIT_FAILURE);
    }
    // pre-processing.
    int c;
    bool have_to_insert_space = false; // write only one single space.
    bool is_first_word = true; // don't include space at the beginning.
    while ((c = fgetc(src)) != EOF) {
        if (!isspace(c) && !ispunct(c)) { // ignore tha spaces and the puntation.
            if (have_to_insert_space && !is_first_word) { // insert a space only if we already write something and we haven't write a space.
                fputc(' ', dst);
                have_to_insert_space = false;
            }
            fputc(tolower(c), dst);
            is_first_word = false;
            have_to_insert_space = false;
        } else {
            have_to_insert_space = true;
        }
    }
    // close the source and destination.
    fclose(dst);
    fclose(src);
}