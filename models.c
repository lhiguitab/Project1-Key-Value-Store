#include "models.h"
#include <stdlib.h>
#include <string.h>

// Global variables
Game *games = NULL;
User *users = NULL;
Review *reviews = NULL;

// Add the date to the hash table
void add_game(int app_id, const char *title, const char *rating, int positive_ratio, int user_reviews) {
    Game *game = malloc(sizeof(Game));
    game->app_id = app_id;
    strncpy(game->title, title, MAX_TITLE);
    strncpy(game->rating, rating, MAX_RATING);
    game->positive_ratio = positive_ratio;
    game->user_reviews = user_reviews;
    HASH_ADD_INT(games, app_id, game);
}

void add_user(int user_id, int reviews_count) {
    User *user = malloc(sizeof(User));
    user->user_id = user_id;
    user->reviews = reviews_count;
    HASH_ADD_INT(users, user_id, user);
}

void add_review(int review_id, int app_id, int user_id) {
    Review *review = malloc(sizeof(Review));
    review->review_id = review_id;
    review->app_id = app_id;
    review->user_id = user_id;
    HASH_ADD_INT(reviews, review_id, review);
}

// Find the date in the hash table
Game *find_game(int app_id) {
    Game *game = NULL;
    HASH_FIND_INT(games, &app_id, game);
    return game;
}

User *find_user(int user_id) {
    User *user = NULL;
    HASH_FIND_INT(users, &user_id, user);
    return user;
}

Review *find_review(int review_id) {
    Review *review = NULL;
    HASH_FIND_INT(reviews, &review_id, review);
    return review;
}
