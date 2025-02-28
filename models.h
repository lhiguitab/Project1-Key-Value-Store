#ifndef MODELS_H
#define MODELS_H

#include "uthash.h"

#define MAX_TITLE 100
#define MAX_RATING 20

// Struct for the games without unnecessary fields
typedef struct {
    int app_id;       // Key
    char title[MAX_TITLE];
    char rating[MAX_RATING];
    int positive_ratio;
    int user_reviews;
    UT_hash_handle hh;  // Required by uthash
} Game;

// Struct for the users without unnecessary fields
typedef struct {
    int user_id;    // Key
    int reviews;
    UT_hash_handle hh;  // Required by uthash
} User;

// Struct for the reviews without unnecessary fields
typedef struct {
    int review_id;  // Key
    int app_id;
    int user_id;
    UT_hash_handle hh;  // Required by uthash
} Review;


// Struct for the user set (used in top of recommended games)
typedef struct {
    int user_id;
    UT_hash_handle hh;
} UserSet;

// Struct to map app_id -> count (used in top recommedation games)
typedef struct {
    int app_id;         // Key
    int count;          // Reviws counter
    UT_hash_handle hh;  // urhash
} AppCount;

// Global variables (extern)
// Hash tables
extern Game *games;       // Tabla hash de juegos (clave: app_id)
extern User *users;       // Tabla hash de usuarios (clave: user_id)
extern Review *reviews;   // Tabla hash de reviews (clave: review_id)


void add_game(int app_id, const char *title, const char *rating, int positive_ratio, int user_reviews);
void add_user(int user_id, int reviews);
void add_review(int review_id, int app_id, int user_id);
Game *find_game(int app_id);
User *find_user(int user_id);
Review *find_review(int review_id);

#endif
