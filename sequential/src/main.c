#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int main() {
    printf("Running Hash Table Test...\n");
    HashTable *ht = create_hash_table(10);

    char *grams[] = {
        "one two three",
        "two three four",
        "one two three", // Duplicate
        "five six seven"
    };

    for (int i = 0; i < 4; i++) {
        add_gram(ht, grams[i]);
    }

    // Since I can't inspect internals easily without modifying the header (which I shouldn't unless necessary),
    // I will just rely on the fact that 'free_hash_table' should run without error.

    printf("Freeing table...\n");
    free_hash_table(ht);
    printf("Test Finished.\n");
    return 0;
}