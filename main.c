#include "store.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#include <stdio.h>
#include <stdlib.h>

void clear_cache() {
    #ifdef __linux__
        int result = system("sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches' > /dev/null 2>&1");
        (void)result;
    #elif defined(_WIN32)
        int result = system("powershell.exe -Command \"Clear-PhysicalMemoryCache\"");
        (void)result;
    #elif defined(__APPLE__)
        int result = system("purge");
        (void)result;
    #else
        printf("Operating system not supported for clearing cache.\n");
    #endif
}


int main(int argc, char *argv[]) {
    // Verifica que se haya pasado la opción -f y un folder
    if (argc != 3 || strcmp(argv[1], "-f") != 0) {
        fprintf(stderr, "Uso: %s -f FOLDER\n", argv[0]);
        return 1;
    }

    char *folder = argv[2];
    char games_file[512], users_file[512], recs_file[512];

    // Construye las rutas de los archivos usando la carpeta proporcionada
    snprintf(games_file, sizeof(games_file), "%s/games.csv", folder);
    snprintf(users_file, sizeof(users_file), "%s/users.csv", folder);
    snprintf(recs_file, sizeof(recs_file), "%s/recommendations.csv", folder);
    
    clear_cache();
    
    // Load dataset
    clock_t start = clock();
    load_recommendations_from_csv("./Dataset/recommendations.csv");
    load_games_from_csv("./Dataset/games.csv");
    load_users_from_csv("./Dataset/users.csv");
    clock_t end = clock();
    
    // Search tops
    clock_t start_search = clock();
    top10_most_recommended_games();
    bottom10_less_recommended_games();
    top10_user_with_most_recommendations();
    games_most_recommended_by_top10();
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
