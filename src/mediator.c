/*
 * Mediator program which
 * controls game between players
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "game.h"
#include "map.h"
#include "units.h"

#define PLAYER_PROGRAM "player"
#define MAX_TURNS 2000

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
int build(Player *player, char type) {
    Unit u;
    // Already building some unit
    if (player->base.building != '0') {
        printf("Base already building\n");
        return 0;
    }
    // Get specific unit for data
    u = unit(-1, -1, -1, type);

    // Check if player has enough gold
    if (player->gold < u.price) {
        printf("Not enough gold\n");
        return 0;
    }

    // Set building if everything is ok
    player->base.building = type;
    player->base.building_duration = u.build_time;
    player->gold -= u.price;
    return 1;
}

// Move action
int move(Player *player, Player *enemy, Map board, int id, int x, int y) {
    Unit *unit = get_unit_by_id(player, id);
    int d = distance(unit->x, unit->y, x, y);

    // Check if unit has enough speed
    if (d > unit->speed) {
        printf("Not enough speed points\n");
        return 0;
    }
    // Unit can't go outside the map
    if (x >= board.no_cols || y >= board.no_rows) {
        printf("Can't go outside the map\n");
        return 0;
    }
    // Unit can't go where obstacle is
    if (board.board_matrix[y][x] == '9') {
        printf("Can't go where obstacle is\n");
        return 0;
    }
    // Unit can't go where enemy base is
    if (x == enemy->base.x && y == enemy->base.y) {
        printf("Can't go where enemy base is\n");
        return 0;
    }
    // Unit can't go where enemy is
    for (int u = 0; u < enemy->no_units; u++) {
        if (x == enemy->units[u].x && y == enemy->units[u].y) {
            printf("Can't go where enemy stands\n");
            return 0;
        }
    }

    // All conditions are passed so place unit where it should be
    unit->x = x;
    unit->y = y;
    unit->speed -= d;
    return 1;
}

// Attack action
int attack(Player *player, Player *enemy, int id, int id_enemy) {
    Unit *attacked;
    int d, dmg;
    Unit *attacking = get_unit_by_id(player, id);

    // Check if speed value is sufficient
    if (attacking->speed < 1) {
        printf("Not enough speed for attack\n");
        return 0;
    }
    // Check if unit already attacked
    if (attacking->attacked != 0) {
        printf("This unit already attacked\n");
        return 0;
    }
    // Check if attacked unit isn't ally
    for (int u = 0; u < player->no_units; u++) {
        if (player->units[u].id == id_enemy) {
            printf("Can't attack ally\n");
            return 0;
        }
    }
    // Check who is attacked
    if (id_enemy == enemy->base.id) {
        // Enemy base is attacked
        // Check if unit has sufficient range
        d = distance(attacking->x, attacking->y, enemy->base.x, enemy->base.y);
        if (d > attacking->range) {
            printf("Unit not in range\n");
            return 0;
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
            return 0;
        }
        dmg = get_damage(attacking->type, attacked->type);
        attacked->durability -= dmg;
    }
    attacking->speed -= 1;
    attacking->attacked = 1;
    return 1;
}

// Process single order
int process_order(Player *player, Player *enemy, Map board, char *tokens[]) {
    int id, x, y, id_enemy, valid;
    char action, type;

    // Parse variables
    id = atoi(tokens[0]);
    // Check if unit exists
    if (get_unit_by_id(player, id) == NULL && player->base.id != id) {
        printf("Unit with given id doesn't exist\n");
        return 0;
    }
    action = *tokens[1];

    valid = 1;
    // Choose action type
    switch (action) {
        case 'B':
            type = *tokens[2];
            valid = build(player, type);
            break;
        case 'M':
            x = atoi(tokens[2]);
            y = atoi(tokens[3]);
            valid = move(player, enemy, board, id, x, y);
            break;
        case 'A':
            id_enemy = atoi(tokens[2]);
            // Check if unit exists
            if (get_unit_by_id(enemy, id_enemy) == NULL && enemy->base.id != id_enemy) {
                printf("Unit with given id doesn't exist\n");
                return 0;
            }
            valid = attack(player, enemy, id, id_enemy);
            break;
        default:
            printf("Unknown action\n");
            valid = 0;
            break;
    }
    return valid;
}

// Process orders given by player in orders.txt file
int process_orders(Player *p1, Player *p2, Map board, int turn, char *orders_filename) {
    FILE *file;
    char buffer[128], *token, *tokens[4];
    Player *player, *enemy;
    int i, valid;

    if ((file = fopen(orders_filename, "r")) == NULL) {
        perror("Failed to open status file");
        exit(EXIT_FAILURE);
    }

    // Set who is player and enemy
    set_players_roles(&p1, &p2, &player, &enemy, turn);

    valid = 1;
    // Iterate through lines of status file
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        i = 0;
        // Use strtok to get individual tokens
        token = strtok(buffer, " \n");
        while (token != NULL) {
            tokens[i++] = token;
            token = strtok(NULL, " \n");
        }
        valid = process_order(player, enemy, board, tokens);
        if (!valid) {
            break;
        }
    }

    fclose(file);
    return valid;
}

// Process turn for one player
void process_turn_player(Player *p, Map board) {
    int u;
    // Build units
    if (p->base.building != '0') {
        // Lower time of builidng
        p->base.building_duration--;
        if (p->base.building_duration == 0) {
            // Build unit on base if time passed
            add_unit(p, unit(id++, p->base.x, p->base.y, p->base.building));
            p->base.building = '0';
        }
    }
    // Clear destroyed units
    u = 0;
    while (u < p->no_units) {
        if (p->units[u].durability < 0) {
            del_unit(p, u);
        } else {
            u++;
        }
    }
    // Add gold if workers on mines
    for (int w = 0; w < p->no_units; w++) {
        // Find worker unit
        if (p->units[w].type == 'W') {
            // Check if worker stays on mine and add gold if he is
            if (board.board_matrix[p->units[w].y][p->units[w].x] == '6') {
                p->gold += 50;
            }
        }
    }
}

// Process turn changes sucha as building or gold mining
void process_turn(Player *p1, Player *p2, Map board, int turn) {
    Player *player, *enemy;
    Unit def_unit;

    // Set who is player and enemy
    set_players_roles(&p1, &p2, &player, &enemy, turn);

    // In one turn process idle actions for both players
    process_turn_player(p1, board);
    process_turn_player(p2, board);

    // Reset speed of units for player who played turn
    for (int u = 0; u < player->no_units; u++) {
        // Get unit of specific type for default data
        def_unit = unit(-1, -1, -1, player->units[u].type);
        player->units[u].speed = def_unit.speed;
        // Reset attacked state
        player->units[u].attacked = 0;
    }
}

// Check result of the game in case of exceeding no. turns
void end_game(Player *p1, Player *p2) {
    if (p1->no_units > p2->no_units) {
        printf("Player 1 won with %d more unit(s)!\n", p1->no_units - p2->no_units);
    } else if (p2->no_units > p1->no_units) {
        printf("Player 2 won with %d more unit(s)!\n", p2->no_units - p1->no_units);
    } else {
        printf("Tie!");
    }
}

// Check if enemy base is destroyed
int player_won(Player *p1, Player *p2, int turn) {
    Player *player, *enemy;
    // Set who is player and enemy
    set_players_roles(&p1, &p2, &player, &enemy, turn);
    return (enemy->base.durability <= 0) ? 1 : 0;
}

int main(int argc, char *argv[]) {
    int status, turn, player_num, valid;
    char *map_filename, *status_filename, *orders_filename, *time_limit_str, program[strlen(PLAYER_PROGRAM) + 6];
    Map board;
    Player player1, player2;
    pid_t pid;
    struct timespec start, stop;
    double time_taken;

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt orders.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    orders_filename = argv[3];
    time_limit_str = (argc == 5) ? argv[4] : "5";

    // Prepare data
    load_map(&board, map_filename);
    set_players(&player1, &player2, &board);
    turn = 1;
    player_num = 1;
    add_unit(&player1, unit(id++, 31, 4, 'K'));
    add_unit(&player1, unit(id++, 30, 2, 'S'));
    add_unit(&player1, unit(id++, 1, 0, 'W'));
    add_unit(&player2, unit(id++, 2, 3, 'A'));
    prepare_status(&player1, &player2, turn, status_filename);

    // Prepare for executing player program
    sprintf(program, "./%s.out", PLAYER_PROGRAM);
    char *args[] = {PLAYER_PROGRAM, map_filename, status_filename, orders_filename, time_limit_str, NULL};
    while (1) {
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
            clock_gettime(CLOCK_REALTIME, &start);
            // Wait for the child process to terminate
            waitpid(pid, &status, 0);
            if (WIFSIGNALED(status)) {
                printf("Player process error\n");
                exit(EXIT_FAILURE);
            }
            clock_gettime(CLOCK_REALTIME, &stop);

            // Set player number
            player_num = (turn % 2 != 0) ? 1 : 2;
            // Calculate the time taken by player
            time_taken = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / 1E9;
            if (time_taken > atof(time_limit_str)) {
                printf("Time limit exceeded.\nPlayer %d won!\n", (player_num == 1) ? 2 : 1);
                break;
            }

            // Process and validate commands
            valid = process_orders(&player1, &player2, board, turn, orders_filename);
            // If player did bad move other one wins
            if (!valid) {
                printf("Player %d won!\n", (player_num == 1) ? 2 : 1);
                break;
            }

            // Process automatic turn actions
            process_turn(&player1, &player2, board, turn);

            // Check if player destroyed enemy base
            if (player_won(&player1, &player2, turn)) {
                printf("Player %d won by destroying enemy base!\n", player_num);
                break;
            }
            // Check if turns limit is exceeded
            ++turn;
            if (turn > MAX_TURNS) {
                // Game has ended so check result
                end_game(&player1, &player2);
                break;
            }

            // Prepare status file for player
            prepare_status(&player1, &player2, turn, status_filename);
        }
    }

    // Free allocated memory
    free_map(&board);
    free_player(&player1);
    free_player(&player2);
    return 0;
}
