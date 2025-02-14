#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE 1024

void readCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("It was not possible to open the file");
        return;
    }

    char linea[MAX_LINE];
    while (fgets(linea, MAX_LINE, file)) {
    //     // Separate the line by commas
        char *token = strtok(linea, ",");
        while (token) {
            printf("%s | ", token);
            token = strtok(NULL, ",");
        }
        printf("\n");
    }
    fclose(file);
}

int main() {
    clock_t start = clock();

    readCSV("./Dataset/recommendations.csv");

    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time);

    return 0;
}
