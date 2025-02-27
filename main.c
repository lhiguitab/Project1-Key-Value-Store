    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    #include <omp.h>

    #include "uthash.h"

    #define MAX_LINE 4096
    #define MAX_LINES 2500000

    #define n_threads 4

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

int main() {
    // set the number of threads to the maximum available for parallel processing
    omp_set_num_threads(omp_get_max_threads());
    clear_cache(); // function to clear cache if necessary

    // TEST - start measuring execution time
    clock_t start = clock();

    // open the dataset file
    const char *filename = "./Dataset/recommendations.csv";
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // allocate memory for storing lines in a batch
    char **lines = (char **)malloc(MAX_LINES * sizeof(char *));
    int batch_count = 0;

    while (1) { // keep reading until the end of the file
        int total_lines = 0;

        // read up to 2,500,000 lines per iteration
        while (total_lines < MAX_LINES) {
            char buffer[MAX_LINE];
            if (!fgets(buffer, sizeof(buffer), file)) break; // end of file
            lines[total_lines] = strdup(buffer); // duplicate line and store it
            total_lines++;
        }

        if (total_lines == 0) break; // if no lines were read, exit loop

        batch_count++;
        printf("Processing batch %d with %d lines...\n", batch_count, total_lines);

        // parallel processing of the batch
        #pragma omp parallel for
        for (int i = 0; i < total_lines; i++) {
            int review_id, app_id, user_id;
            char *token = strtok(lines[i], ","); // extract first value (app_id)
            if (token) {
                app_id = atoi(token); // convert to integer
                for (int j = 0; j < 5; j++) token = strtok(NULL, ","); // skip next 5 values
                token = strtok(NULL, ","); // extract user_id
                if (token) {
                    user_id = atoi(token);
                    token = strtok(NULL, ","); // extract review_id
                    if (token) {
                        review_id = atoi(token);

                        // allocate memory for a new review and store values
                        Review *review = (Review *)malloc(sizeof(Review));
                        review->review_id = review_id;
                        review->app_id = app_id;
                        review->user_id = user_id;

                        // critical section to safely add review to hash table
                        #pragma omp critical
                        HASH_ADD_INT(reviews, review_id, review);
                    }
                }
            }
            free(lines[i]); // free memory of processed line
        }
    }

    free(lines); // free buffer memory
    fclose(file); // close file

    // TEST - end measuring execution time
    clock_t end = clock();
    printf("Execution time: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}

    