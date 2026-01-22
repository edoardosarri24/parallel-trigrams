#include <stdlib.h>
#include <sys/mman.h>
#include <omp.h>
#include "hash_table.h"
#include "main_functions.h"
#include "statistics.h"
#include "my_utils.h"

int main() {
    // total time start.
    double start_time = omp_get_wtime();

    // open the input file
    const char *input_filepath = DATA_DIR"/input.txt";
    size_t input_size = 0;
    const char *input_data = map_file(input_filepath, &input_size);

    // populate the hash table.
    double start_time_populate = omp_get_wtime();
    HashTable *hashTable = populate_hashtable(input_data, input_size);
    munmap((void *)input_data, input_size);
    double end_time_populate = omp_get_wtime();

    // statistics.
    double start_time_statistics = omp_get_wtime();
    printf("\n----- TEXT STATISTICS -----\n");
    TextStatistics text_stats = calculate_text_statistics(hashTable);
    if (text_stats.unique_ngrams == 0) {
        printf("No n-grams found.\n");
    } else {
        printf("Total unique %d-grams: %zu\n", N_GRAM_SIZE, text_stats.unique_ngrams);
        printf("Top %d n-grams:\n", TOP_K);
        for (int i = 0; i < text_stats.top_filled; ++i) {
            printf("%d - '%s': %d\n", i + 1, text_stats.top_ngrams[i]->gram, text_stats.top_ngrams[i]->counter);
        }
    }
    printf("\n----- HASHTABLE STATISTICS -----\n");
    HashTableStatistics ht_stats = calculate_hashtable_statistics(hashTable);
    printf("-Total buckets: %d\n", ht_stats.buckets_size);
    printf("-Busy buckets: %zu\n", ht_stats.busy_buckets);
    printf("-Total elements: %zu\n", ht_stats.total_elements);
    printf("-Max chain length: %zu\n", ht_stats.max_chain_len);
    printf("-Avg chain length: %.2f\n", ht_stats.busy_buckets > 0 ? ((double)ht_stats.total_elements / ht_stats.busy_buckets) : 0.0);
    printf("-Load Factor (elements/buckets): %.4f\n", (double)ht_stats.total_elements / ht_stats.buckets_size);
    printf("-Fill Factor (busy/total): %.4f\n", (double)ht_stats.busy_buckets / ht_stats.buckets_size);
    double end_time_statistics = omp_get_wtime();

    // free the hash table.
    double start_time_free = omp_get_wtime();
    free_hash_table(hashTable);
    double end_time_free = omp_get_wtime();

    // total time end.
    double end_time = omp_get_wtime();

    // print time statistics.
    printf("\n----- TIME STATISTICS -----\n");
    printf("Total execution time: %.4f seconds\n", end_time - start_time);
    printf("Populate hashtable time: %.4f seconds\n", end_time_populate - start_time_populate);
    printf("Statistics time: %.4f seconds\n", end_time_statistics - start_time_statistics);
    printf("Free hashtable time: %.4f seconds\n", end_time_free - start_time_free);

    // return.
    return EXIT_SUCCESS;
}