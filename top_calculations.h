#ifndef TOP_CALCULATIONS_H
#define TOP_CALCULATIONS_H

#include "models.h"

void top10_most_recommended_games(void);
void bottom10_less_recommended_games(void);
User* top_users_by_reviews(int top_n, int *out_size);
void top10_user_with_most_recommendations(void);
void games_most_recommended_by_top10(void);

#endif
