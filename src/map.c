// File with map utilities

#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Loads map from file into 2D matrix
void load_map(Map *board, char *filename) {
    int rows, cols;
    char buffer[MAX_COLS];
    FILE *file;

    if ((file = fopen(filename, "r")) == NULL) {
        perror("Failed to open map file");
        exit(EXIT_FAILURE);
    }

    // Count the number of rows
    rows = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        rows++;
    }
    // Count the number of columns
    cols = strlen(buffer);

    // Set the dimensions of the map
    board->no_rows = rows;
    board->no_cols = cols;

    // Reset the file pointer to the beginning
    fseek(file, 0, SEEK_SET);

    // Allocate map rows
    board->board_matrix = malloc(board->no_rows * sizeof(char *));
    for (int r = 0; r < board->no_rows; r++) {
        // Allocate map column
        board->board_matrix[r] = malloc((board->no_cols + 1) * sizeof(char));
        if (fgets(buffer, board->no_cols + 2, file) == NULL) {
            perror("Error reading map data");
            exit(EXIT_FAILURE);
        }
        memcpy(board->board_matrix[r], buffer, board->no_cols + 1);
        // Null-terminate the string
        board->board_matrix[r][board->no_cols] = '\0';
    }

    fclose(file);
}

// Prints 2D matrix representing map
void print_map(Map *board) {
    printf("-----Map-----\n");
    printf("Number of rows: %d   Number of columns: %d\n", board->no_rows, board->no_cols);
    for (int i = 0; i < board->no_rows; i++) {
        printf("%s\n", board->board_matrix[i]);
    }
}

// Deallocate map memory
void free_map(Map *board) {
    for (int i = 0; i < board->no_rows; i++) {
        free(board->board_matrix[i]);
    }
    free(board->board_matrix);
}
