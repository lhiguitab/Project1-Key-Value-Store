#include "top_calculations.h"
#include "models.h"
#include <stdio.h>
#include <stdlib.h>

// Methods to convert hash tables to arrays
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

// Comparators for qsort
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

// Top 10 most recommended games (by user_reviews)
void top10_most_recommended_games(void) {
    int count;
    Game *arr = games_to_array(&count);
    if (count == 0) {
        printf("No loaded games.\n");
        return;
    }
    qsort(arr, count, sizeof(Game), cmp_games_desc_by_user_reviews);
    int top = (count < 10) ? count : 10;
    printf("\n=== Top %d most recommended games\n", top);
    for (int i = 0; i < top; i++) {
        printf("%d) %s - app_id:%d - user_reviews:%d\n",
               i + 1, arr[i].title, arr[i].app_id, arr[i].user_reviews);
    }
    free(arr);
}

// Bottom 10 less recommended games (by user_reviews)
void bottom10_less_recommended_games(void) {
    int count;
    Game *arr = games_to_array(&count);
    if (count == 0) {
        printf("No loaded games.\n");
        return;
    }
    qsort(arr, count, sizeof(Game), cmp_games_asc_by_user_reviews);
    int bottom = (count < 10) ? count : 10;
    printf("\nBottom %d less recommended games\n", bottom);
    for (int i = 0; i < bottom; i++) {
        printf("%d) %s - app_id:%d - user_reviews=%d\n",
               i + 1, arr[i].title, arr[i].app_id, arr[i].user_reviews);
    }
    free(arr);
}

// Returns an array with the top users by reviews
User* top_users_by_reviews(int top_n, int *out_size) {
    int count;
    User *arr = users_to_array(&count);
    if (count == 0) {
        *out_size = 0;
        return NULL;
    }
    qsort(arr, count, sizeof(User), cmp_users_desc_by_reviews);
    *out_size = (count < top_n) ? count : top_n;
    return arr;  
}

// Top 10 users with most recommendations
void top10_user_with_most_recommendations(void) {
    int top_n = 10;
    int out_size;
    User *top_users = top_users_by_reviews(top_n, &out_size);
    if (out_size == 0) {
        free(top_users);
        printf("\nIt wasn't possible obtain the top.\n");
        return;
    }
    printf("\nTop %d users with most recommendations\n", top_n);
    for (int i = 0; i < out_size; i++) {
        printf("%d) User ID: %d - Reviews: %d\n",
               i + 1, top_users[i].user_id, top_users[i].reviews);
    }
    free(top_users);
}

// Most recommended games by top 10 users
void games_most_recommended_by_top10(void) {
    int top_n = 10;
    int out_size;
    User *top_users = top_users_by_reviews(top_n, &out_size);
    if (out_size == 0) {
        free(top_users);
        printf("\nIt wasn't possible obtain the top.\n");
        return;
    }
    int *top_user_ids = malloc(out_size * sizeof(int));
    for (int i = 0; i < out_size; i++) {
        top_user_ids[i] = top_users[i].user_id;
    }
    free(top_users);
    
    // Create a hash set of this users
    UserSet *top_set = NULL;
    for (int i = 0; i < out_size; i++) {
        UserSet *us = malloc(sizeof(UserSet));
        us->user_id = top_user_ids[i];
        HASH_ADD_INT(top_set, user_id, us);
    }
    free(top_user_ids);

    /// Iterate the reviews table and count the recommendations by app_id only for users in top_set
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

    // Free the hash set
    UserSet *us, *us_tmp;
    HASH_ITER(hh, top_set, us, us_tmp) {
        HASH_DEL(top_set, us);
        free(us);
    }

    // Create an array of AppCount pointers for sorting
    int count_items = HASH_COUNT(counts);
    if (count_items == 0) {
        printf("\nThere are no reviews from the top users.\n");
        return;
    }
    AppCount **ac_array = malloc(count_items * sizeof(AppCount *));
    int idx = 0;
    AppCount *ac, *ac_tmp;
    HASH_ITER(hh, counts, ac, ac_tmp) {
        ac_array[idx++] = ac;
    }
    // Comparator for qsort: descending order by count
    int cmp_appcount_desc(const void *a, const void *b) {
        AppCount *ac1 = *(AppCount **)a;
        AppCount *ac2 = *(AppCount **)b;
        return (ac2->count - ac1->count);
    }
    qsort(ac_array, count_items, sizeof(AppCount *), cmp_appcount_desc);
    int top_games = (count_items < 3) ? count_items : 3;
    printf("\nTop %d most recommended games by top %d users\n", top_games, top_n);
    for (int i = 0; i < top_games; i++) {
        Game *g = find_game(ac_array[i]->app_id);
        if (g) {
            printf("%d) App ID: %d | Title: %s | Recommendationss among top %d users: %d\n",
                   i + 1, g->app_id, g->title, top_n, ac_array[i]->count);
        } else {
            printf("%d) App ID: %d (no info in 'games') - Recs: %d\n",
                i + 1, ac_array[i]->app_id, ac_array[i]->count);
        }
    }
    free(ac_array);
    // Free the counts hash
    HASH_ITER(hh, counts, ac, ac_tmp) {
        HASH_DEL(counts, ac);
        free(ac);
    }
}
