#include "store.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <omp.h>

void clear_cache() {
    #ifdef __linux__
        int result = system("sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches' > /dev/null 2>&1");
        (void)result;
    #elif defined(_WIN32)
        printf("Operating system not supported for clearing cache.\n");
    #elif defined(__APPLE__)
        int result = system("purge");
        (void)result;
    #else
        printf("Operating system not supported for clearing cache.\n");
    #endif
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-f") != 0) {
        fprintf(stderr, "Uso: %s -f FOLDER\n", argv[0]);
        return 1;
    }

    char *folder = argv[2];

    /* Verifica que el folder exista y sea un directorio */
    struct stat st;
    if (stat(folder, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: La carpeta '%s' no existe o no es un directorio.\n", folder);
        return 1;
    }

    /* Opcional: verifica que existan los archivos dentro del folder */
    char games_file[512], users_file[512], recs_file[512];
    snprintf(games_file, sizeof(games_file), "%s/games.csv", folder);
    snprintf(users_file, sizeof(users_file), "%s/users.csv", folder);
    snprintf(recs_file, sizeof(recs_file), "%s/recommendations.csv", folder);

    if (stat(games_file, &st) != 0) {
        fprintf(stderr, "Error: El archivo '%s' no existe.\n", games_file);
        return 1;
    }
    if (stat(users_file, &st) != 0) {
        fprintf(stderr, "Error: El archivo '%s' no existe.\n", users_file);
        return 1;
    }
    if (stat(recs_file, &st) != 0) {
        fprintf(stderr, "Error: El archivo '%s' no existe.\n", recs_file);
        return 1;
    }
    
    clear_cache();
    
    // Load dataset
    clock_t start = clock();
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            load_recommendations_from_csv(recs_file);
        }
        #pragma omp section
        {
            load_games_from_csv(games_file);
        }
        #pragma omp section
        {
            load_users_from_csv(users_file);
        }
    }
    clock_t end = clock();
    
    // Search tops
    clock_t start_search = clock();
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            top10_most_recommended_games();
        }
        #pragma omp section
        {
            bottom10_less_recommended_games();
        }
        #pragma omp section
        {
            top10_user_with_most_recommendations();
        }
        #pragma omp section
        {
            games_most_recommended_by_top10();
        }
    }
    clock_t end_search = clock();
    
    // Free memory
    free_reviews();
    free_games();
    free_users();
    
    double load_time = (double)(end - start) / CLOCKS_PER_SEC;
    double search_time = (double)(end_search - start_search) / CLOCKS_PER_SEC;
    
    printf("\nExecution time: %f seconds\n", load_time);
    printf("Search time: %f seconds\n", search_time);
    
    return 0;
}
