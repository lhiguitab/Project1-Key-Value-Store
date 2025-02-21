#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "uthash.h"
// #include "hash_table.h"

#define MAX_LINE 4096

void clear_cache() {
    system("sudo su -c \"echo 3 > /proc/sys/vm/drop_caches\" > /dev/null 2>&1");
}

// Define the structure of the CSV file
typedef struct {
    char **lines;
    int line_count;
} CSVFile;

typedef struct {
    int app_id;       // Key
    char title[100];
    char rating[20];
    int positive_ratio;
    int user_reviews;
    UT_hash_handle hh;  // Required for uthash
} Game;

typedef struct {
    int user_id;    // Key
    int reviews;
    UT_hash_handle hh;  // Required for uthash
} User;

typedef struct {
    int review_id;  // Key
    int app_id;
    int user_id;
    UT_hash_handle hh;  // Required for uthash
} Review;

// Declare games as a global variable
Game *games = NULL;  // Hash table (key: app_id)
User *users = NULL;  // Hash table (key: user_id)
Review *reviews = NULL;  // Hash table (key: review_id)

void add_game(int app_id, const char *title, const char *rating, int positive_ratio, int user_reviews) {
    Game *game = malloc(sizeof(Game));
    game->app_id = app_id;
    strncpy(game->title, title, sizeof(game->title));
    strncpy(game->rating, rating, sizeof(game->rating));
    game->positive_ratio = positive_ratio;
    game->user_reviews = user_reviews;
    HASH_ADD_INT(games, app_id, game);  // Add to hash table
}

void add_user(int user_id, int reviews) {
    User *user = malloc(sizeof(User));
    user->user_id = user_id;
    user->reviews = reviews;
    HASH_ADD_INT(users, user_id, user);  // Add to hash table
}

void add_review(int review_id, int app_id, int user_id) {
    Review *review = malloc(sizeof(Review));
    review->review_id = review_id;
    review->app_id = app_id;
    review->user_id = user_id;
    HASH_ADD_INT(reviews, review_id, review);  // Add to hash table
}

Game *find_game(int app_id) {
    Game *game;
    HASH_FIND_INT(games, &app_id, game);
    return game;
}

User *find_user(int user_id) {
    User *user;
    HASH_FIND_INT(users, &user_id, user);
    return user;
}

Review *find_review(int review_id) {
    Review *review;
    HASH_FIND_INT(reviews, &review_id, review);
    return review;
}

void print_games() {
    Game *game, *tmp;
    HASH_ITER(hh, games, game, tmp) {
        printf("Game: %d - %s (%s)\n", game->app_id, game->title, game->rating);
    }
}

void free_games() {
    Game *game, *tmp;
    HASH_ITER(hh, games, game, tmp) {
        HASH_DEL(games, game);
        free(game);
    }
}

void free_users() {
    User *user, *tmp;
    HASH_ITER(hh, users, user, tmp) {
        HASH_DEL(users, user);
        free(user);
    }
}

void free_reviews() {
    Review *review, *tmp;
    HASH_ITER(hh, reviews, review, tmp) {
        HASH_DEL(reviews, review);
        free(review);
    }
}

void load_games_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE];  // Buffer for each line
    while (fgets(line, sizeof(line), file)) {
        int app_id, positive_ratio, user_reviews;
        char title[100], rating[20];

        // Parse CSV line (assuming format: app_id,title,rating,positive_ratio,user_reviews)
        if (sscanf(line, "%d,%99[^,],%19[^,],%d,%d", &app_id, title, rating, &positive_ratio, &user_reviews) == 5) {
            add_game(app_id, title, rating, positive_ratio, user_reviews);
        }
    }
    fclose(file);
}

void load_users_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE];  // Buffer for each line
    while (fgets(line, sizeof(line), file)) {
        int user_id, reviews;

        // Parse CSV line (assuming format: user_id,reviews)
        if (sscanf(line, "%d,%d", &user_id, &reviews) == 2) {
            add_user(user_id, reviews);
        }
    }
    fclose(file);
}

void load_recommendations_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE];  // Buffer for each line
    while (fgets(line, sizeof(line), file)) {
        int review_id, app_id, user_id;

        // Parse CSV line (assuming format: review_id,app_id,user_id)
        if (sscanf(line, "%d,%d,%d", &review_id, &app_id, &user_id) == 3) {
            add_review(review_id, app_id, user_id);
        }
    }
    fclose(file);
}

int main() {
    // Clear the cache
    clear_cache();
    
    CSVFile csv_files[3] = {
        {"./Dataset/games.csv", 0},
        {"./Dataset/players.csv", 0},
        {"./Dataset/reviews.csv", 0}
    };
    
    // Game *games = NULL;  // Hash table (key: app_id)
    // User *users = NULL;  // Hash table (key: user_id)
    // Review *reviews = NULL;  // Hash table (key: review_id)
    
    void (*f)(char**, CSVFile*) = NULL;
    char *folder = NULL;

    clock_t start = clock();

    load_recommendations_from_csv("./Dataset/recommendations.csv");
    // load_games_from_csv("./Dataset/games.csv");
    // load_users_from_csv("./Dataset/users.csv");

    clock_t end = clock();

    clock_t start_search = clock();
    find_review(1938090);
    clock_t end_search = clock();

    free_reviews();

    double time = (double)(end - start) / CLOCKS_PER_SEC;
    double time_search = (double)(end_search - start_search) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", time);
    printf("Search time: %f seconds\n", time_search);

    return 0;
}