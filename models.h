#ifndef MODELS_H
#define MODELS_H

#include "uthash.h"

#define MAX_TITLE 100
#define MAX_RATING 20

/* Estructura para juegos */
typedef struct {
    int app_id;       // Clave
    char title[MAX_TITLE];
    char rating[MAX_RATING];
    int positive_ratio;
    int user_reviews;
    UT_hash_handle hh;  // Requerido por uthash
} Game;

/* Estructura para usuarios */
typedef struct {
    int user_id;    // Clave
    int reviews;
    UT_hash_handle hh;  // Requerido por uthash
} User;

/* Estructura para reviews */
typedef struct {
    int review_id;  // Clave
    int app_id;
    int user_id;
    UT_hash_handle hh;  // Requerido por uthash
} Review;

/* Estructura para construir un hash set de usuarios (usado en top cálculos) */
typedef struct {
    int user_id;
    UT_hash_handle hh;
} UserSet;

/* Estructura para mapear app_id -> conteo (usado en top de juegos recomendados) */
typedef struct {
    int app_id;         // clave
    int count;          // cuántas recomendaciones
    UT_hash_handle hh;  // para uthash
} AppCount;

/* Variables globales (extern) */
extern Game *games;       // Tabla hash de juegos (clave: app_id)
extern User *users;       // Tabla hash de usuarios (clave: user_id)
extern Review *reviews;   // Tabla hash de reviews (clave: review_id)

/* Prototipos para funciones básicas (agregar y buscar) */
void add_game(int app_id, const char *title, const char *rating, int positive_ratio, int user_reviews);
void add_user(int user_id, int reviews);
void add_review(int review_id, int app_id, int user_id);
Game *find_game(int app_id);
User *find_user(int user_id);
Review *find_review(int review_id);

#endif /* MODELS_H */
