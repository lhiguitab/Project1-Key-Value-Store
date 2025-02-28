#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "uthash.h"

#define MAX_LINE 4096

/* -------------------- ESTRUCTURAS -------------------- */

/* Estructura para juegos */
typedef struct {
    int app_id;       // Clave
    char title[100];
    char rating[20];
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

/* Estructura para construir un hash set de usuarios */
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

/* -------------------- VARIABLES GLOBALES -------------------- */
Game *games = NULL;       // Tabla hash de juegos (clave: app_id)
User *users = NULL;       // Tabla hash de usuarios (clave: user_id)
Review *reviews = NULL;   // Tabla hash de reviews (clave: review_id)

/* -------------------- PROTOTIPOS -------------------- */
void clear_cache(void);
void add_game(int app_id, const char *title, const char *rating, int positive_ratio, int user_reviews);
void add_user(int user_id, int reviews);
void add_review(int review_id, int app_id, int user_id);
Game *find_game(int app_id);
User *find_user(int user_id);
Review *find_review(int review_id);
void load_games_from_csv(const char *filename);
void load_users_from_csv(const char *filename);
void load_recommendations_from_csv(const char *filename);
void free_games(void);
void free_users(void);
void free_reviews(void);
Game* games_to_array(int *count);
User* users_to_array(int *count);
int cmp_games_desc_by_user_reviews(const void *a, const void *b);
int cmp_games_asc_by_user_reviews(const void *a, const void *b);
int cmp_users_desc_by_reviews(const void *a, const void *b);
void top10_most_recommended_games(void);
void bottom10_less_recommended_games(void);
User* top_users_by_reviews(int top_n, int *out_size);
void user_with_most_recommendations(void);
void games_most_recommended_by_top10(void);

/* -------------------- FUNCIONES -------------------- */

/* Opcional: limpia la caché del sistema (requiere permisos de root) */
void clear_cache(void) {
    int result = system("sudo su -c \"echo 3 > /proc/sys/vm/drop_caches\" > /dev/null 2>&1");
    (void)result;
}

/* Inserta un juego en la hash table 'games' */
void add_game(int app_id, const char *title, const char *rating, int positive_ratio, int user_reviews) {
    Game *game = malloc(sizeof(Game));
    game->app_id = app_id;
    strncpy(game->title, title, sizeof(game->title));
    strncpy(game->rating, rating, sizeof(game->rating));
    game->positive_ratio = positive_ratio;
    game->user_reviews = user_reviews;
    HASH_ADD_INT(games, app_id, game);
}

/* Inserta un usuario en la hash table 'users' */
void add_user(int user_id, int reviews_count) {
    User *user = malloc(sizeof(User));
    user->user_id = user_id;
    user->reviews = reviews_count;
    HASH_ADD_INT(users, user_id, user);
}

/* Inserta una review en la hash table 'reviews' */
void add_review(int review_id, int app_id, int user_id) {
    Review *review = malloc(sizeof(Review));
    review->review_id = review_id;
    review->app_id = app_id;
    review->user_id = user_id;
    HASH_ADD_INT(reviews, review_id, review);
}

/* Búsqueda en 'games' */
Game *find_game(int app_id) {
    Game *game = NULL;
    HASH_FIND_INT(games, &app_id, game);
    return game;
}

/* Búsqueda en 'users' */
User *find_user(int user_id) {
    User *user = NULL;
    HASH_FIND_INT(users, &user_id, user);
    return user;
}

/* Búsqueda en 'reviews' */
Review *find_review(int review_id) {
    Review *review = NULL;
    HASH_FIND_INT(reviews, &review_id, review);
    return review;
}

/* Carga datos desde games.csv.
   Se asume que el CSV tiene el formato:
   app_id,title,date,win,mac,linux,rating,positive_ratio,user_reviews
   (Se ignoran 4 tokens intermedios: date, win, mac, linux)
*/
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
        char title[100], rating[20];
        char *token = strtok(line, ","); // app_id
        if (token) {
            app_id = atoi(token);
            token = strtok(NULL, ","); // title
            if (token) {
                strncpy(title, token, sizeof(title));
                /* Saltamos 4 tokens: date, win, mac, linux */
                for (int i = 0; i < 4; i++) {
                    token = strtok(NULL, ",");
                    if (!token) break;
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

/* Carga datos desde users.csv.
   Se asume el formato: user_id,products,reviews
*/
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

/* Carga datos desde recommendations.csv.
   Se asume el formato:
   app_id,helpful,funny,date,is_recommended,hours,user_id,review_id
   Se ignoran los 5 tokens intermedios.
*/
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
                    if (count % 1000000 == 0) {
                        printf("Loaded %ld recommendations\n", count);
                    }
                }
            }
        }
    }
    fclose(file);
}

/* Libera la memoria de la tabla hash de juegos */
void free_games(void) {
    Game *g, *tmp;
    HASH_ITER(hh, games, g, tmp) {
        HASH_DEL(games, g);
        free(g);
    }
}

/* Libera la memoria de la tabla hash de usuarios */
void free_users(void) {
    User *u, *tmp;
    HASH_ITER(hh, users, u, tmp) {
        HASH_DEL(users, u);
        free(u);
    }
}

/* Libera la memoria de la tabla hash de reviews */
void free_reviews(void) {
    Review *r, *tmp;
    HASH_ITER(hh, reviews, r, tmp) {
        HASH_DEL(reviews, r);
        free(r);
    }
}

/* Convierte la tabla hash de juegos en un arreglo */
Game* games_to_array(int *count) {
    *count = 0;
    Game *g, *tmp;
    HASH_ITER(hh, games, g, tmp) {
        (*count)++;
    }
    Game *arr = malloc((*count) * sizeof(Game));
    int i = 0;
    HASH_ITER(hh, games, g, tmp) {
        arr[i++] = *g;
    }
    return arr;
}

/* Convierte la tabla hash de usuarios en un arreglo */
User* users_to_array(int *count) {
    *count = 0;
    User *u, *tmp;
    HASH_ITER(hh, users, u, tmp) {
        (*count)++;
    }
    User *arr = malloc((*count) * sizeof(User));
    int i = 0;
    HASH_ITER(hh, users, u, tmp) {
        arr[i++] = *u;
    }
    return arr;
}

/* Comparador descendente para juegos según user_reviews */
int cmp_games_desc_by_user_reviews(const void *a, const void *b) {
    const Game *ga = (const Game *)a;
    const Game *gb = (const Game *)b;
    return (gb->user_reviews - ga->user_reviews);
}

/* Comparador ascendente para juegos según user_reviews */
int cmp_games_asc_by_user_reviews(const void *a, const void *b) {
    const Game *ga = (const Game *)a;
    const Game *gb = (const Game *)b;
    return (ga->user_reviews - gb->user_reviews);
}

/* Comparador descendente para usuarios según reviews */
int cmp_users_desc_by_reviews(const void *a, const void *b) {
    const User *ua = (const User *)a;
    const User *ub = (const User *)b;
    return (ub->reviews - ua->reviews);
}

/* -------------------- FUNCIONES PARA TOPS -------------------- */

/* Top 10 juegos más recomendados (por user_reviews) */
void top10_most_recommended_games(void) {
    int count;
    Game *arr = games_to_array(&count);
    if (count == 0) {
        printf("No hay juegos cargados.\n");
        return;
    }
    qsort(arr, count, sizeof(Game), cmp_games_desc_by_user_reviews);
    int top = (count < 10) ? count : 10;
    printf("\n=== Top %d Juegos más recomendados (por user_reviews) ===\n", top);
    for (int i = 0; i < top; i++) {
        printf("%d) %s (app_id=%d) - user_reviews=%d\n",
               i + 1, arr[i].title, arr[i].app_id, arr[i].user_reviews);
    }
    free(arr);
}

/* Bottom 10 juegos menos recomendados (por user_reviews) */
void bottom10_less_recommended_games(void) {
    int count;
    Game *arr = games_to_array(&count);
    if (count == 0) {
        printf("No hay juegos cargados.\n");
        return;
    }
    qsort(arr, count, sizeof(Game), cmp_games_asc_by_user_reviews);
    int bottom = (count < 10) ? count : 10;
    printf("\n=== Bottom %d Juegos menos recomendados (por user_reviews) ===\n", bottom);
    for (int i = 0; i < bottom; i++) {
        printf("%d) %s (app_id=%d) - user_reviews=%d\n",
               i + 1, arr[i].title, arr[i].app_id, arr[i].user_reviews);
    }
    free(arr);
}

/* Devuelve un arreglo de usuarios ordenado descendentemente por reviews.
   Se consideran solo los top_n usuarios; se indica en out_size cuántos hay realmente.
*/
User* top_users_by_reviews(int top_n, int *out_size) {
    int count;
    User *arr = users_to_array(&count);
    if (count == 0) {
        *out_size = 0;
        return NULL;
    }
    qsort(arr, count, sizeof(User), cmp_users_desc_by_reviews);
    *out_size = (count < top_n) ? count : top_n;
    return arr;  // Se devuelve el arreglo completo; se usan los primeros out_size elementos.
}

/* Muestra el top 10 usuarios con más recomendaciones (reviews) */
void top10_user_with_most_recommendations(void) {
    int top_n = 10;
    int out_size;
    User *top_users = top_users_by_reviews(top_n, &out_size);
    if (out_size == 0) {
        free(top_users);
        printf("\nNo hay usuarios o no se pudo obtener el top.\n");
        return;
    }
    printf("\n=== Top %d Usuarios con más recomendaciones ===\n", top_n);
    for (int i = 0; i < out_size; i++) {
        printf("%d) User ID: %d - Reviews: %d\n", i + 1, top_users[i].user_id, top_users[i].reviews);
    }
    free(top_users);
}

/* Juegos más recomendados por los Top 10 usuarios.
   Se obtiene el top 10 de usuarios (por reviews), se crea un hash set con sus IDs,
   y se recorren las reviews para contar las recomendaciones por app_id.
*/
void games_most_recommended_by_top10(void) {
    int top_n = 10;
    int out_size;
    User *top_users = top_users_by_reviews(top_n, &out_size);
    if (out_size == 0) {
        free(top_users);
        printf("\nNo hay usuarios o no se pudo obtener el top.\n");
        return;
    }
    int *top_user_ids = malloc(out_size * sizeof(int));
    for (int i = 0; i < out_size; i++) {
        top_user_ids[i] = top_users[i].user_id;
    }
    free(top_users);
    /* Creamos un hash set de esos user_id */
    UserSet *top_set = NULL;
    for (int i = 0; i < out_size; i++) {
        UserSet *us = malloc(sizeof(UserSet));
        us->user_id = top_user_ids[i];
        HASH_ADD_INT(top_set, user_id, us);
    }
    free(top_user_ids);
    /* Recorremos la tabla de reviews y contamos, solo si el review proviene de un usuario en top_set */
    Review *rev, *rev_tmp;
    AppCount *counts = NULL;
    HASH_ITER(hh, reviews, rev, rev_tmp) {
        UserSet *u_check;
        HASH_FIND_INT(top_set, &rev->user_id, u_check);
        if (u_check) {
            AppCount *ac;
            HASH_FIND_INT(counts, &rev->app_id, ac);
            if (!ac) {
                ac = malloc(sizeof(AppCount));
                ac->app_id = rev->app_id;
                ac->count = 0;
                HASH_ADD_INT(counts, app_id, ac);
            }
            ac->count += 1;
        }
    }
    /* Liberamos el top_set */
    UserSet *us, *us_tmp;
    HASH_ITER(hh, top_set, us, us_tmp) {
        HASH_DEL(top_set, us);
        free(us);
    }
    /* Buscamos el máximo conteo */
    int max_count = 0;
    AppCount *ac, *ac_tmp;
    HASH_ITER(hh, counts, ac, ac_tmp) {
        if (ac->count > max_count) {
            max_count = ac->count;
        }
    }
    printf("\n=== Juego(s) más recomendado(s) por los Top %d usuarios ===\n", top_n);
    HASH_ITER(hh, counts, ac, ac_tmp) {
        if (ac->count == max_count) {
            Game *g = find_game(ac->app_id);
            if (g) {
                printf("App ID: %d | Title: %s | Recs among top %d users: %d\n",
                       g->app_id, g->title, top_n, ac->count);
            } else {
                printf("App ID: %d (no info in 'games') - Recs: %d\n", ac->app_id, ac->count);
            }
        }
    }
    HASH_ITER(hh, counts, ac, ac_tmp) {
        HASH_DEL(counts, ac);
        free(ac);
    }
}

/* -------------------- MAIN -------------------- */

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
