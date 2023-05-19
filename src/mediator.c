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
#include "units.h"

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
    player->base.building = '0';
    player->no_units = 0;
    player->units = NULL;
}

// Set players structs starting game
void set_players(Player *p1, Player *p2, Map *board) {
    for (int r = 0; r < board->no_rows; r++) {
        for (int c = 0; c < board->no_cols; c++) {
            if (board->board_matrix[r][c] == '1') {
                set_player(p1, c, r);
            } else if (board->board_matrix[r][c] == '2') {
                set_player(p2, c, r);
            }
        }
    }
}

// Prepare status file based on data
void prepare_status(Player *p1, Player *p2, int turn, char *filename) {
    FILE *file = fopen(filename, "w");
    Player *player, *enemy;
    if (file == NULL) {
        perror("Failed to open status file");
        exit(EXIT_FAILURE);
    }

    // Set player and enemy
    if (turn % 2 != 0) {
        // Player 1 turn
        player = p1;
        enemy = p2;
    } else {
        // Player 2 turn
        player = p2;
        enemy = p1;
    }

    // Gold
    fprintf(file, "%d\n", player->gold);
    // Player base
    fprintf(file, "P B %d %d %d %d %c\n",
            player->base.id, player->base.x, player->base.y,
            player->base.durability, player->base.building);
    // Enemy base
    fprintf(file, "E B %d %d %d %d %c\n",
            enemy->base.id, enemy->base.x, enemy->base.y,
            enemy->base.durability, enemy->base.building);
    // Player units
    for (int u = 0; u < player->no_units; u++) {
        fprintf(file, "P %c %d %d %d %d\n",
                player->units[u].type, player->units[u].id, player->units[u].x,
                player->units[u].y, player->units[u].durability);
    }
    // Enemy units
    for (int u = 0; u < enemy->no_units; u++) {
        fprintf(file, "P %c %d %d %d %d\n",
                enemy->units[u].type, enemy->units[u].id, enemy->units[u].x,
                enemy->units[u].y, enemy->units[u].durability);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    int status, running, turn;
    char *map_filename, *status_filename, *commands_filename, *time_limit, program[strlen(PLAYER_PROGRAM) + 6];
    Map board;
    Player player1, player2;
    pid_t pid;

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt commands.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    commands_filename = argv[3];
    time_limit = (argc == 5) ? argv[4] : "5";

    printf("Map file: %s   Status file: %s   Commands file: %s   Time limit: %s\n\n",
           map_filename, status_filename, commands_filename, time_limit);

    // Prepare data
    load_map(&board, map_filename);
    set_players(&player1, &player2, &board);
    turn = 1;
    prepare_status(&player1, &player2, turn++, status_filename);

    // Prepare for executing player program
    sprintf(program, "./%s.out", PLAYER_PROGRAM);
    char *args[] = {PLAYER_PROGRAM, map_filename, status_filename, commands_filename, time_limit, NULL};
    running = 1;
    while (running) {
        // Fork a new process
        if ((pid = fork()) < 0) {
            perror("Failed to fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process (player program)

            // Execute the player program
            execvp(program, args);

            perror("Failed to execute the player program");
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

            add_unit(&player2, knight(id++, 31, 4));
            // Prepare status file for player
            prepare_status(&player1, &player2, turn++, status_filename);

            running = 0;
        }
    }

    free_map(&board);
    free_player(&player1);
    free_player(&player2);
    return 0;
}
