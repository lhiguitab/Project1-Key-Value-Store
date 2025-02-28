#ifndef CSV_LOADER_H
#define CSV_LOADER_H

/* Prototipos para la carga y liberación de datos desde CSV */
void load_games_from_csv(const char *filename);
void load_users_from_csv(const char *filename);
void load_recommendations_from_csv(const char *filename);
void free_games(void);
void free_users(void);
void free_reviews(void);

#endif /* CSV_LOADER_H */
