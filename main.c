#include "store.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

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
        fprintf(stderr, "Use: %s -f FOLDER\n", argv[0]);
        return 1;
    }

    char *folder = argv[2];

    // Check if the folder exists
    struct stat st;
    if (stat(folder, &st) != 0 || !S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: The folder '%s' doesn't exist or is not a directory.\n", folder);
        return 1;
    }

    // Check if the files exist
    char games_file[512], users_file[512], recs_file[512];
    snprintf(games_file, sizeof(games_file), "%s/games.csv", folder);
    snprintf(users_file, sizeof(users_file), "%s/users.csv", folder);
    snprintf(recs_file, sizeof(recs_file), "%s/recommendations.csv", folder);

    if (stat(games_file, &st) != 0) {
        fprintf(stderr, "Error: The file '%s' doesn't exists.\n", games_file);
        return 1;
    }
    if (stat(users_file, &st) != 0) {
        fprintf(stderr, "Error: The file '%s' doesn't exists.\n", users_file);
        return 1;
    }
    if (stat(recs_file, &st) != 0) {
        fprintf(stderr, "Error: The file '%s' doesn't exists.\n", recs_file);
        return 1;
    }
    
    clear_cache();
    
    // Load dataset
    clock_t start = clock();
    load_recommendations_from_csv(recs_file);
    load_games_from_csv(games_file);
    load_users_from_csv(users_file);
    clock_t end = clock();
    
    // Search tops
    // Menu
    int option = 0;
    do {
        printf("\n1. Top 10 most recommended games\n");
        printf("2. Bottom 10 less recommended games\n");
        printf("3. Top 10 users with most recommendations\n");
        printf("4. Games most recommended by top 10 users\n");
        printf("5. All tops and search time\n");
        printf("6. Exit\n");
        printf("Option: ");
        scanf("%d", &option);
        switch (option) {
            case 1:
            top10_most_recommended_games(); // Top 10 most recommended games
            break;
            case 2:
            bottom10_less_recommended_games(); // Bottom 10 less recommended games
            break;
            case 3:
            top10_user_with_most_recommendations(); // Top 10 users with most recommendations
            break;
            case 4:
            games_most_recommended_by_top10(); // Games most recommended by top 10 users
            break;
            case 5:
                clock_t start_search = clock();
                top10_most_recommended_games();
                bottom10_less_recommended_games();
                top10_user_with_most_recommendations();
                games_most_recommended_by_top10();
                clock_t end_search = clock();

                double search_time = (double)(end_search - start_search) / CLOCKS_PER_SEC;
                printf("Search time: %f seconds\n", search_time);
                break;
            case 6:
                break;
            default:
                printf("Invalid option\n");
        }
    } while (option != 6);
    
    // Free memory
    free_reviews();
    free_games();
    free_users();
    
    double load_time = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("\nExecution time: %f seconds\n", load_time);
    
    return 0;
}
