/*
File with map utilities
*/

#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Loads map from file into 2D matrix */
void loadMap(Map *board, char *filename)
{
    int rows, cols;
    char buffer[MAX_COLS];
    FILE *file;

    if ((file = fopen(filename, "r")) == NULL)
    {
        printf("Failed to open the file.\n");
        exit(EXIT_FAILURE);
    }

    // Count the number of rows
    rows = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
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
    for (int i = 0; i < board->no_rows; i++)
    {
        // Allocate map column
        board->board_matrix[i] = malloc((board->no_cols + 1) * sizeof(char));
        if (fgets(buffer, board->no_cols + 2, file) == NULL)
        {
            printf("Error reading map data.\n");
            exit(EXIT_FAILURE);
        }
        memcpy(board->board_matrix[i], buffer, board->no_cols + 1);
        // Null-terminate the string
        board->board_matrix[i][board->no_cols] = '\0';
    }

    fclose(file);
}

/* Prints 2D matrix representing map*/
void printMap(Map *board)
{
    printf("-----Map-----\n");
    printf("Number of rows: %d   Number of columns: %d\n", board->no_rows, board->no_cols);
    for (int i = 0; i < board->no_rows; i++)
    {
        printf("%s\n", board->board_matrix[i]);
    }
}

/* Deallocate map memory */
void freeMap(Map *board)
{
    for (int i = 0; i < board->no_rows; i++)
    {
        free(board->board_matrix[i]);
    }
    free(board->board_matrix);
}
