#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>

#include <omp.h>

#include "uthash.h"

#define MAX_LINE 4096
#define MAX_LINES 42000000

#define BLOCK_SIZE 4096

#define n_threads 12

void clear_cache() {
    //int result = system("sudo su -c \"echo 3 > /proc/sys/vm/drop_caches\" > /dev/null 2>&1");
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

// Declare the hash tables as a global variables
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

void print_users(){
    User *user, *tmp;
    HASH_ITER(hh, users, user, tmp) {
        printf("User: %d - %d\n", user->user_id, user->reviews);
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

    printf("Loading games from CSV file...\n");
    
    char line[MAX_LINE];  // Buffer for each line

    //int temp = strcmp(filename, "./Dataset/games.csv");
    char* temp = strcmp(filename, "./Dataset/games.csv") ;


    while (fgets(line, sizeof(line), file)) {
        int app_id, positive_ratio, user_reviews;
        char title[100], rating[20];

        // Parse CSV line (assuming format: app_id,title,rating,positive_ratio,user_reviews)
        char *token = strtok(line, ","); // app_id
        if (token) {
            app_id = atoi(token);
            token = strtok(NULL, ","); // title
            if (token) {
                strncpy(title, token, sizeof(title));
                for (int i = 0; i < 4; i++) {
                    token = strtok(NULL, ","); //date, win, mac, linux
                }
                token = strtok(NULL, ","); // rating
                if (token) {
                    strncpy(rating, token, sizeof(rating));
                    token = strtok(NULL, ","); // positive_ratio
                    if (token) {
                        positive_ratio = atoi(token);
                        token = strtok(NULL, ","); // user_reviews
                        if (token) {
                            user_reviews = atoi(token);
                            add_game(app_id, title, rating, positive_ratio, user_reviews);
                        }
                    }
                }
            }
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
        char *token = strtok(line, ","); // user_id
        if (token) {
            user_id = atoi(token);
            token = strtok(NULL, ","); // products
            token = strtok(NULL, ","); // reviews
            if (token) {
                reviews = atoi(token);
                add_user(user_id, reviews);
            }
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
        char *token = strtok(line, ","); // app_id
        
        if (token) {
            app_id = atoi(token);
            for (int i = 0; i < 5; i++) token = strtok(NULL, ","); // helpful, funny, date, is_recommended, hours
            token = strtok(NULL, ","); // user_id
            if (token) {
                user_id = atoi(token);
                token = strtok(NULL, ","); // review_id
                if (token) {
                    review_id = atoi(token);
                    add_review(review_id, app_id, user_id);
                }
            }
        }
    }
    fclose(file);
}


// this is a variation of load_recommendations_from_csv
// the original version remains intact, so tests are performed using that function in the main program

void process_block(char *block, size_t size, Review **reviews2) {
    char *start = block;
    int is_first_line = 1;
    
    for (size_t i = 0; i < size; i++) {
        if (block[i] == '\n' || i == size - 1) {
            block[i] = '\0';

            char *token = strtok(start, ",");
            if (!token) continue;

            if (is_first_line) { 
                if (strcmp(token, "app_id") == 0) { 
                    start = &block[i + 1]; 
                    is_first_line = 0; // saltar encabezado
                    continue;
                }
            }

            int app_id = atoi(token);

            for (int j = 0; j < 5; j++) { // saltar hasta la columna 6 (user_id)
                token = strtok(NULL, ",");
                if (!token) break;
            }
            if (!token) continue;

            int user_id = atoi(token);

            token = strtok(NULL, ","); // obtener review_id (columna 7)
            if (!token) continue;
            int review_id = atoi(token);

            Review *review = (Review *)malloc(sizeof(Review));
            if (!review) continue;
            review->review_id = review_id;
            review->app_id = app_id;
            review->user_id = user_id;

            // agregar a la hash table en sección crítica
            #pragma omp critical
            HASH_ADD_INT(*reviews2, review_id, review);

            start = &block[i + 1];
        }
    }
}

void print_reviews(Review *reviews) {
    Review *current_review, *tmp;
    int i = 0;
    HASH_ITER(hh, reviews, current_review, tmp) {
        printf("Review ID: %d, App ID: %d, User ID: %d, i: %d\n", 
               current_review->review_id, current_review->app_id, current_review->user_id, i);
        i++;
    }
}


int main() {
    Review *reviews2 = NULL;

    omp_set_num_threads(omp_get_max_threads());
    clock_t start_time = clock();

    const char *filename = "./Dataset/recommendations.csv";
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return 1;
    }

    size_t filesize = sb.st_size;
    char *file_map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    size_t line_count = 0;
    for (size_t i = 0; i < filesize; i++) {
        if (file_map[i] == '\n') line_count++;
    }
    printf("Total lines detected: %zu\n", line_count);

        #pragma omp parallel for schedule(dynamic)
    for (size_t offset = 0; offset < filesize; offset += BLOCK_SIZE) {
        size_t block_size = (offset + BLOCK_SIZE < filesize) ? BLOCK_SIZE : (filesize - offset);
        process_block(file_map + offset, block_size, &reviews2);
    }

    print_reviews(reviews2);


    printf("test\n");

    Review *found_review = NULL;
    int search_id = 0; // review_id que queremos buscar
    
    HASH_FIND_INT(reviews, &search_id, found_review);
    
    if (found_review) {
        printf("Review encontrada: app_id=%d, user_id=%d, review_id=%d\n",
               found_review->app_id, found_review->user_id, found_review->review_id);
    } else {
        printf("Review con review_id=%d no encontrada\n", search_id);
    }
    

    munmap(file_map, filesize);
    close(fd);

    clock_t end = clock();
    printf("Execution time: %f seconds\n", (double)(end - start_time) / CLOCKS_PER_SEC);
    return 0;
}