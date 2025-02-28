#include "top_calculations.h"
#include "models.h"
#include <stdio.h>
#include <stdlib.h>

/* Funciones de conversión: convertir tablas hash a arreglos */
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

/* Comparadores para qsort */
int cmp_games_desc_by_user_reviews(const void *a, const void *b) {
    const Game *ga = (const Game *)a;
    const Game *gb = (const Game *)b;
    return (gb->user_reviews - ga->user_reviews);
}

int cmp_games_asc_by_user_reviews(const void *a, const void *b) {
    const Game *ga = (const Game *)a;
    const Game *gb = (const Game *)b;
    return (ga->user_reviews - gb->user_reviews);
}

int cmp_users_desc_by_reviews(const void *a, const void *b) {
    const User *ua = (const User *)a;
    const User *ub = (const User *)b;
    return (ub->reviews - ua->reviews);
}

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

/* Retorna un arreglo de los usuarios con mayor cantidad de reviews */
User* top_users_by_reviews(int top_n, int *out_size) {
    int count;
    User *arr = users_to_array(&count);
    if (count == 0) {
        *out_size = 0;
        return NULL;
    }
    qsort(arr, count, sizeof(User), cmp_users_desc_by_reviews);
    *out_size = (count < top_n) ? count : top_n;
    return arr;  // El llamador debe liberar el arreglo
}

/* Muestra el top 10 usuarios con más recomendaciones */
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
        printf("%d) User ID: %d - Reviews: %d\n",
               i + 1, top_users[i].user_id, top_users[i].reviews);
    }
    free(top_users);
}

/* Muestra el usuario con más recomendaciones */
void user_with_most_recommendations(void) {
    User *u, *tmp;
    User *max_user = NULL;
    int max_reviews = -1;
    HASH_ITER(hh, users, u, tmp) {
        if (u->reviews > max_reviews) {
            max_reviews = u->reviews;
            max_user = u;
        }
    }
    printf("\n=== Usuario con más recomendaciones ===\n");
    if (max_user) {
        printf("User ID: %d - Reviews: %d\n", max_user->user_id, max_user->reviews);
    } else {
        printf("No hay usuarios cargados.\n");
    }
}

/* Juegos más recomendados por los Top 10 usuarios */
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
    /* Recorremos la tabla de reviews y contamos las recomendaciones por app_id solo de los usuarios en top_set */
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
    /* Liberamos el hash set */
    UserSet *us, *us_tmp;
    HASH_ITER(hh, top_set, us, us_tmp) {
        HASH_DEL(top_set, us);
        free(us);
    }
    /* Buscamos el máximo conteo */
    int max_count = 0;
    AppCount *ac, *ac_tmp;
    HASH_ITER(hh, counts, ac, ac_tmp) {
        if (ac->count > max_count)
            max_count = ac->count;
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
