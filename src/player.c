// Player program representing game player

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "map.h"

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
        player->base = base(id, x, y, durability, *tokens[6], 0);
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
void give_orders(char *orders_filename, Player player, Player enemy, Map board) {
    FILE *file;
    if ((file = fopen(orders_filename, "w")) == NULL) {
        perror("Failed to open orders file");
        exit(EXIT_FAILURE);
    }

    // TODO: Using some strategy decide on orders
    fprintf(file, "%d B A\n", player.base.id);
    // fprintf(file, "%d M %d %d\n",
    //         player.units[0].id, player.units[0].x - 1, player.units[0].y - 1);
    fprintf(file, "%d A %d\n",
            player.units[0].id, enemy.units[0].id);

    fclose(file);
}

int main(int argc, char *argv[]) {
    int time_limit;
    char *map_filename, *status_filename, *orders_filename;
    Map board;
    Player player, enemy;

    // Get passed arguments
    if (argc < 4) {
        printf("Usage: ./<program_name> map.txt status.txt orders.txt [time_limit]\n");
        exit(EXIT_FAILURE);
    }
    map_filename = argv[1];
    status_filename = argv[2];
    orders_filename = argv[3];
    time_limit = (argc == 5) ? atoi(argv[4]) : 5;

    printf("Map file: %s   Status file: %s   Orders file: %s   Time limit: %d\n",
           map_filename, status_filename, orders_filename, time_limit);

    // Load data
    load_map(&board, map_filename);
    // print_map(&board);
    set_players(&player, &enemy);
    load_status(status_filename, &player, &enemy);

    // Given game data give orders accordingly
    give_orders(orders_filename, player, enemy, board);

    // Free allocated memory
    free_player(&player);
    free_player(&enemy);
    free_map(&board);
    return 0;
}
