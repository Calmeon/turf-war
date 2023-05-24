#ifndef MAP_H
#define MAP_H

#define MAX_COLS 256

// Struct containing 2D map matrix and dimensions
typedef struct {
    char **board_matrix;
    int no_rows, no_cols;
} Map;

void load_map(Map *board, char *filename);
void print_map(Map board);
void free_map(Map *board);
int have_mines(Map board);

#endif
