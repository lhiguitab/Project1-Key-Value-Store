#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash_table.h"

#define MAX_LINE 4096

void readCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("It was not possible to open the file");
        return;
    }

    char line[MAX_LINE];
    int firstLine = 1; // Flag to skip the first line
    
    while (fgets(line, MAX_LINE, file)) {
        if (firstLine) {
            firstLine = 0; // Skip the first line
            continue;
        }

        // Separate the line by commas
        char *key = strtok(line, ","); // The first value is the key
        char *value = strtok(NULL, "\n"); // The second value is the value
/*         while (key) {
            printf("%s | ", key);
            key = strtok(NULL, ",");
        }
        printf("\n"); */

        if (key && value) {
            insert(key, value);
        }
    }
    fclose(file);
}

int main() {
    clock_t start = clock();

    readCSV("./Dataset/recommendations.csv");

    char *result = search("708450");
    if (result) {
        printf("Value: %s\n", result);
    } else {
        printf("Key not found\n");
    }

    // Free the memory used by the hash table
    //free_table();

    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time);

    return 0;
}