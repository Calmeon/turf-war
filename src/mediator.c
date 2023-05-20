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
    player->no_units = 0;
    player->units = NULL;
    player->base = base(id++, base_x, base_y, 200, '0');
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
    FILE *file;
    Player *player, *enemy;
    if ((file = fopen(filename, "w")) == NULL) {
        perror("Failed to open status file");
        exit(EXIT_FAILURE);
    }

    // Set player and enemy
    if (turn % 2 != 0) {
        // Player 1 turn
        printf("Player 1 turn\n");
        player = p1;
        enemy = p2;
    } else {
        // Player 2 turn
        printf("Player 2 turn\n");
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
        fprintf(file, "E %c %d %d %d %d\n",
                enemy->units[u].type, enemy->units[u].id, enemy->units[u].x,
                enemy->units[u].y, enemy->units[u].durability);
    }

    fclose(file);
}

// Build action
void build(Player *player, char type) {}

// Move action
void move(Player *player, Player *enemy, Map board, int id, int x, int y) {}

// Attack action
void attack(Player *player, Player *enemy, int id, int id_enemy) {}

// Process single order
void process_order(Player *player, Player *enemy, Map board, char *tokens[]) {
    int id, x, y, id_enemy;
    char action, type;

    // Parse variables
    id = atoi(tokens[0]);
    action = *tokens[1];

    switch (action) {
        case 'B':
            type = *tokens[2];
            build(player, type);
            break;
        case 'M':
            x = atoi(tokens[2]);
            y = atoi(tokens[3]);
            move(player, enemy, board, id, x, y);
            break;
        case 'A':
            id_enemy = atoi(tokens[2]);
            attack(player, enemy, id, id_enemy);
            break;
        default:
            break;
    }
}

// Process orders given by player in orders.txt file
void process_orders(Player *p1, Player *p2, Map board, int turn, char *orders_filename) {
    FILE *file;
    char buffer[128], *token, *tokens[4];
    Player *player, *enemy;
    int i;

    if ((file = fopen(orders_filename, "r")) == NULL) {
        perror("Failed to open status file");
        exit(EXIT_FAILURE);
    }

    // Set player and enemy
    player = (turn % 2 != 0) ? p1 : p2;
    enemy = (turn % 2 != 0) ? p2 : p1;

    // Iterate through lines of status file
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        i = 0;
        // Use strtok to get individual tokens
        token = strtok(buffer, " \n");
        while (token != NULL) {
            tokens[i++] = token;
            token = strtok(NULL, " \n");
        }
        process_order(player, enemy, board, tokens);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    int status, running, turn;
    char *map_filename, *status_filename, *orders_filename, *time_limit, program[strlen(PLAYER_PROGRAM) + 6];
    Map board;
    Player player1, player2;
    pid_t pid;

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt orders.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    orders_filename = argv[3];
    time_limit = (argc == 5) ? argv[4] : "5";

    printf("Map file: %s   Status file: %s   Orders file: %s   Time limit: %s\n",
           map_filename, status_filename, orders_filename, time_limit);

    // Prepare data
    load_map(&board, map_filename);
    set_players(&player1, &player2, &board);
    turn = 1;
    add_unit(&player1, knight(id++, 31, 4));
    add_unit(&player1, swordsman(id++, 30, 2));
    add_unit(&player2, archer(id++, 2, 3));
    prepare_status(&player1, &player2, turn, status_filename);

    // Prepare for executing player program
    sprintf(program, "./%s.out", PLAYER_PROGRAM);
    char *args[] = {PLAYER_PROGRAM, map_filename, status_filename, orders_filename, time_limit, NULL};
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
            if (WIFSIGNALED(status)) {
                printf("Player process error\n");
                exit(EXIT_FAILURE);
            }
            /*
             * TODO:
             * Check if in time limit
             * Process and validate commands
             * Check ending conditions
             */
            process_orders(&player1, &player2, board, turn, orders_filename);

            // Prepare status file for player
            // prepare_status(&player1, &player2, ++turn, status_filename);

            running = 0;
        }
    }

    // Free allocated memory
    free_map(&board);
    free_player(&player1);
    free_player(&player2);
    return 0;
}
