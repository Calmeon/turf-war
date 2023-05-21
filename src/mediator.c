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
    player->base = base(id++, base_x, base_y, 200, '0', 0);
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

// Based on turn set player and enemy
void set_players_roles(Player **p1, Player **p2, Player **player, Player **enemy, int turn) {
    // If turn is odd number then there is p1 turn otherwise p2
    *player = (turn % 2 != 0) ? *p1 : *p2;
    *enemy = (turn % 2 != 0) ? *p2 : *p1;
}

// Prepare status file based on data
void prepare_status(Player *p1, Player *p2, int turn, char *filename) {
    FILE *file;
    Player *player, *enemy;

    if ((file = fopen(filename, "w")) == NULL) {
        perror("Failed to open status file");
        exit(EXIT_FAILURE);
    }

    // Set who is player and enemy
    set_players_roles(&p1, &p2, &player, &enemy, turn);

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
void build(Player *player, char type) {
    Unit u;
    // Already building some unit
    if (player->base.building != '0') {
        printf("Base already building\n");
        return;
    }
    // Get specific unit for data
    u = unit(-1, -1, -1, type);

    // Check if player has enough gold
    if (player->gold < u.price) {
        printf("Not enough gold\n");
        return;
    }

    // Set building if everything is ok
    player->base.building = type;
    player->base.building_duration = u.build_time;
    player->gold -= u.price;
}

// Move action
void move(Player *player, Player *enemy, Map board, int id, int x, int y) {
    Unit *unit = get_unit_by_id(player, id);
    int d = distance(unit->x, unit->y, x, y);

    // Check if unit has enough speed
    if (d > unit->speed) {
        printf("Not enough speed points\n");
        return;
    }
    // Unit can't go outside the map
    if (x >= board.no_cols || y >= board.no_rows) {
        printf("Can't go outside the map\n");
        return;
    }
    // Unit can't go where obstacle is
    if (board.board_matrix[y][x] == '9') {
        printf("Can't go where obstacle is\n");
        return;
    }
    // Unit can't go where enemy base is
    if (x == enemy->base.x && y == enemy->base.y) {
        printf("Can't go where enemy base is\n");
        return;
    }
    // Unit can't go where enemy is
    for (int u = 0; u < enemy->no_units; u++) {
        if (x == enemy->units[u].x && y == enemy->units[u].y) {
            printf("Can't go where enemy stands\n");
            return;
        }
    }

    // All conditions are passed so place unit where it should be
    unit->x = x;
    unit->y = y;
    unit->speed -= d;
}

// Attack action
void attack(Player *player, Player *enemy, int id, int id_enemy) {
    Unit *attacked;
    int d, dmg;
    Unit *attacking = get_unit_by_id(player, id);

    // Check if speed value is sufficient
    if (attacking->speed < 1) {
        printf("Not enough speed for attack\n");
        return;
    }
    // Check if unit already attacked
    if (attacking->attacked != 0) {
        printf("This unit already attacked\n");
        return;
    }
    // Check who is attacked
    if (id_enemy == enemy->base.id) {
        // Enemy base is attacked
        // Check if unit has sufficient range
        d = distance(attacking->x, attacking->y, enemy->base.x, enemy->base.y);
        if (d > attacking->range) {
            printf("Unit not in range\n");
            return;
        }
        dmg = get_damage(attacking->type, 'B');
        enemy->base.durability -= dmg;
    } else {
        // Enemy unit is attacked
        attacked = get_unit_by_id(enemy, id_enemy);
        // Check if unit has sufficient range
        d = distance(attacking->x, attacking->y, attacked->x, attacked->y);
        if (d > attacking->range) {
            printf("Unit not in range\n");
            return;
        }
        dmg = get_damage(attacking->type, attacked->type);
        attacked->durability -= dmg;
    }
    attacking->speed -= 1;
}

// Process single order
void process_order(Player *player, Player *enemy, Map board, char *tokens[]) {
    int id, x, y, id_enemy;
    char action, type;

    // Parse variables
    id = atoi(tokens[0]);
    action = *tokens[1];

    // Choose action type
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
            printf("Unknown action\n");
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

    // Set who is player and enemy
    set_players_roles(&p1, &p2, &player, &enemy, turn);

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

// Process turn changes sucha as building or gold mining
void process_turn(Player *player1, Player *player2, Map board, int turn) {
    /*
     * TODO:
     * Build units/substract time of builidng
     * Add gold if workers on mines
     * Reset speed of units
     */
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
    add_unit(&player1, unit(id++, 31, 4, 'K'));
    add_unit(&player1, unit(id++, 30, 2, 'S'));
    add_unit(&player2, unit(id++, 2, 3, 'A'));
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
             * Check ending conditions
             */
            // Process and validate commands
            process_orders(&player1, &player2, board, turn, orders_filename);
            // Process automatic turn actions
            process_turn(&player1, &player2, board, turn);
            // Prepare status file for player
            prepare_status(&player1, &player2, ++turn, status_filename);

            running = 0;
        }
    }

    // Free allocated memory
    free_map(&board);
    free_player(&player1);
    free_player(&player2);
    return 0;
}
