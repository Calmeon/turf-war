/*
 * Mediator program which
 * controls game between players
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "game.h"
#include "map.h"

#define PLAYER_PROGRAM "player"

// Global id parameter to keep track on id given
int id = 0;

// Set default parameters for player and set position of the base
void set_player(Player *player, int base_x, int base_y) {
    player->gold = 2000;
    player->base.id = id++;
    player->base.durability = 200;
    player->base.x = base_x;
    player->base.y = base_y;
}

// Set players structs starting game
void set_players(Player *player1, Player *player2, Map *board) {
    for (int r = 0; r < board->no_rows; r++) {
        for (int c = 0; c < board->no_cols; c++) {
            if (board->board_matrix[r][c] == '1') {
                set_player(player1, c, r);
            } else if (board->board_matrix[r][c] == '2') {
                set_player(player2, c, r);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int time_limit, status, running;
    char *map_filename, *status_filename, *commands_filename, program[strlen(PLAYER_PROGRAM) + 6];
    Map board;
    Player player1, player2;
    pid_t pid;

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt commands.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", argv[0]);
    map_filename = argv[1];
    status_filename = argv[2];
    commands_filename = argv[3];
    time_limit = (argc == 5) ? atoi(argv[4]) : 5;

    printf("Map file: %s   Status file: %s   Commands file: %s   Time limit: %d\n\n",
           map_filename, status_filename, commands_filename, time_limit);

    // Prepare data
    load_map(&board, map_filename);
    set_players(&player1, &player2, &board);

    // Prepare for executing player program
    sprintf(program, "./%s.out", PLAYER_PROGRAM);
    char *args[] = {PLAYER_PROGRAM, map_filename, status_filename, commands_filename, NULL};
    running = 1;
    while (running) {
        // Fork a new process
        if ((pid = fork()) < 0) {
            printf("Failed to fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process (player program)

            // Execute the player program
            execvp(program, args);

            perror("Failed to execute the player program\n");
            exit(EXIT_FAILURE);
        } else {
            // Parent process (mediator program)

            // Wait for the child process to terminate
            waitpid(pid, &status, 0);

            /*
             * TODO:
             * Check if in time limit
             * Check ending conditions
             * Process and validate commands
             */

            running = 0;
        }
    }

    free_map(&board);
    return 0;
}
