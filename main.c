#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE 50874

void leerCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return;
    }

    char linea[MAX_LINE];
    while (fgets(linea, MAX_LINE, file)) {
        // Separar los valores por comas
        // char *token = strtok(linea, ",");
        // while (token) {
        //     printf("%s | ", token);
        //     token = strtok(NULL, ",");
        // }
        // printf("\n");
    }
    fclose(file);
}

int main() {
    clock_t inicio = clock();

    leerCSV("./Dataset/games.csv");

    clock_t fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución: %f segundos\n", tiempo);

    return 0;
}
