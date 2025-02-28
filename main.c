#include "store.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#include <stdio.h>
#include <stdlib.h>

void clear_cache() {
    #ifdef __linux__
        // En Linux, limpiamos la caché con el comando adecuado.
        int result = system("sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches' > /dev/null 2>&1");
        (void)result;  // Evitamos warning por variable no usada.
    #elif defined(_WIN32)
        // En Windows no existe un comando equivalente simple para limpiar la caché del sistema.
        // Podrías emitir un mensaje o implementar otra lógica si es necesario.
        printf("Clearing cache is not implemented for Windows.\n");
    #elif defined(__APPLE__)
        // En macOS se puede utilizar el comando 'purge' para liberar la caché.
        int result = system("purge");
        (void)result;
    #else
        printf("Operating system not supported for clearing cache.\n");
    #endif
}


int main(void) {
    clear_cache();
    
    clock_t start = clock();
    load_recommendations_from_csv("./Dataset/recommendations.csv");
    load_games_from_csv("./Dataset/games.csv");
    load_users_from_csv("./Dataset/users.csv");
    clock_t end = clock();
    
    clock_t start_search = clock();
    top10_most_recommended_games();
    bottom10_less_recommended_games();
    top10_user_with_most_recommendations();
    user_with_most_recommendations();
    games_most_recommended_by_top10();
    clock_t end_search = clock();
    
    free_reviews();
    free_games();
    free_users();
    
    double load_time = (double)(end - start) / CLOCKS_PER_SEC;
    double search_time = (double)(end_search - start_search) / CLOCKS_PER_SEC;
    
    printf("\nExecution time: %f seconds\n", load_time);
    printf("Search time: %f seconds\n", search_time);
    
    return 0;
}
