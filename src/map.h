#ifndef MAP_H
#define MAP_H

#define MAX_COLS 256

/* Struct containing 2D map matrix and dimensions */
typedef struct
{
    char **board_matrix;
    int no_rows, no_cols;
} Map;

void loadMap(Map *board, char *filename);
void printMap(Map *board);
void freeMap(Map *board);

#endif
