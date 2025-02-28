#include "csv_loader.h"
#include "models.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096 // Buffer to read lines from CSV files

void load_games_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening games file");
        return;
    }
    printf("Loading games from CSV file...\n");
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        int app_id, positive_ratio, user_reviews;
        char title[MAX_TITLE], rating[MAX_RATING];
        char *token = strtok(line, ","); // app_id
        if (token) {
            app_id = atoi(token);
            token = strtok(NULL, ","); // title
            if (token) {
                strncpy(title, token, MAX_TITLE);
                /* Saltamos 4 tokens: date, win, mac, linux */
                for (int i = 0; i < 4; i++) {
                    token = strtok(NULL, ",");
                    if (!token) break;
                }
                token = strtok(NULL, ","); // rating
                if (token) {
                    strncpy(rating, token, MAX_RATING);
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
        perror("Error opening users file");
        return;
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        int user_id, reviews_count;
        char *token = strtok(line, ","); // user_id
        if (token) {
            user_id = atoi(token);
            token = strtok(NULL, ",");   // products (se ignora)
            token = strtok(NULL, ",");   // reviews
            if (token) {
                reviews_count = atoi(token);
                add_user(user_id, reviews_count);
            }
        }
    }
    fclose(file);
}

void load_recommendations_from_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening recommendations file");
        return;
    }
    char line[MAX_LINE];
    long count = 0;
    while (fgets(line, sizeof(line), file)) {
        int review_id, app_id, user_id;
        char *token = strtok(line, ","); // app_id
        if (token) {
            app_id = atoi(token);
            /* Saltamos 5 tokens: helpful, funny, date, is_recommended, hours */
            for (int i = 0; i < 5; i++) token = strtok(NULL, ",");
            token = strtok(NULL, ","); // user_id
            if (token) {
                user_id = atoi(token);
                token = strtok(NULL, ","); // review_id
                if (token) {
                    review_id = atoi(token);
                    add_review(review_id, app_id, user_id);
                    count++;
                    if (count % 1000000 == 0)
                        printf("Loaded %ld recommendations\n", count);
                }
            }
        }
    }
    fclose(file);
}

void free_games(void) {
    Game *g, *tmp;
    HASH_ITER(hh, games, g, tmp) {
        HASH_DEL(games, g);
        free(g);
    }
}

void free_users(void) {
    User *u, *tmp;
    HASH_ITER(hh, users, u, tmp) {
        HASH_DEL(users, u);
        free(u);
    }
}

void free_reviews(void) {
    Review *r, *tmp;
    HASH_ITER(hh, reviews, r, tmp) {
        HASH_DEL(reviews, r);
        free(r);
    }
}
