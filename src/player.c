// Player program representing game player

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "game.h"
#include "map.h"
#include "strategy.h"

// Struct to store allocated resources in order to free them
typedef struct {
    Player *player, *enemy;
    Map *board;
    FILE *file;
} Resources;
Resources global_resources = {.player = NULL, .enemy = NULL, .board = NULL, .file = NULL};

// Handle timeout: clear resources
void handle_timeout(int signum) {
    printf("Program timed out\n");
    free_player(global_resources.player);
    free_player(global_resources.enemy);
    free_map(global_resources.board);
    if (global_resources.file != NULL) {
        fclose(global_resources.file);
    }
    exit(0);
}

// Set default parameters for player
void set_player(Player *player) {
    player->gold = 0;
    player->no_units = 0;
    player->units = NULL;
}

// Set default players structs
void set_players(Player *p1, Player *p2) {
    set_player(p1);
    set_player(p2);
}

// From tokens create and assign unit
void load_unit(char *tokens[], Player *player, Player *enemy) {
    Player *actual_player;
    Unit u;
    int id, x, y, durability;
    char type = *tokens[1];

    // Check whose unit is this
    actual_player = (strcmp(tokens[0], "P") == 0) ? player : enemy;

    // Convert data
    id = atoi(tokens[2]);
    x = atoi(tokens[3]);
    y = atoi(tokens[4]);
    durability = atoi(tokens[5]);

    // Check type of unit and create it
    if (type == 'B') {
        actual_player->base = base(id, x, y, durability, *tokens[6], 0);
        return;
    }
    u = unit(id, x, y, type);

    // Set durability and add unit
    u.durability = durability;
    add_unit(actual_player, u);
}

// Decypher every line and assign it accordingly
void load_status(char *status_filename, Player *player, Player *enemy) {
    char buffer[128], *token, *tokens[7];
    FILE *file;
    int i;

    if ((file = fopen(status_filename, "r")) == NULL) {
        perror("Failed to open status file");
        exit(EXIT_FAILURE);
    }
    global_resources.file = file;

    // Get gold
    fgets(buffer, sizeof(buffer), file);
    player->gold = atoi(buffer);

    // Iterate through lines of status file
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        i = 0;
        // Use strtok to get individual tokens
        token = strtok(buffer, " \n");
        while (token != NULL) {
            tokens[i++] = token;
            token = strtok(NULL, " \n");
        }

        load_unit(tokens, player, enemy);
    }

    fclose(file);
}

// Given all data prepare orders.txt file with moves
void give_orders(char *orders_filename, Player *player, Player *enemy, Map board, float time_limit) {
    FILE *file;
    if ((file = fopen(orders_filename, "w")) == NULL) {
        perror("Failed to open orders file");
        exit(EXIT_FAILURE);
    }
    global_resources.file = file;

    // TODO: Using some strategy decide on orders
    build_strategy(file, player, board);
    attack_strategy(file, player, enemy, board);
    move_strategy(file, player, enemy, board);

    fclose(file);
}

int main(int argc, char *argv[]) {
    double time_limit;
    char *map_filename, *status_filename, *orders_filename;
    Map board;
    Player player, enemy;
    struct itimerval timer;

    // Set a seed for the random number generator
    srand(getpid());
    // Install signal handler for alarm signal
    signal(SIGALRM, handle_timeout);

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt orders.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    orders_filename = argv[3];
    time_limit = (argc == 5) ? atof(argv[4]) : 5;

    // Set up the timer
    // Set time_limit - 0.01 ex. time_limit=5 sets timer for 4.99s
    // 0.01s is safe limit for program cleanup
    timer.it_value.tv_sec = time_limit - 1;
    timer.it_value.tv_usec = 99 * 1E4;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    // Load data
    load_map(&board, map_filename);
    set_players(&player, &enemy);
    load_status(status_filename, &player, &enemy);
    // Pass allocated resources to global to clear them at timeout
    global_resources.player = &player;
    global_resources.enemy = &enemy;
    global_resources.board = &board;

    // Given game data give orders accordingly
    give_orders(orders_filename, &player, &enemy, board, time_limit);

    // Cancel the timer if executed in time
    setitimer(ITIMER_REAL, NULL, NULL);
    // Free allocated memory
    free_player(&player);
    free_player(&enemy);
    free_map(&board);
    return 0;
}
