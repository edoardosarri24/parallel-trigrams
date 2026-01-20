#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <stdio.h>

char *next_ngram(FILE *src);
void preprocess_file(const char *input_filepath, const char *output_filepath);

#endif // HELPER_FUNCTIONS_H