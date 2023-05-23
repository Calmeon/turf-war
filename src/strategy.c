#include "strategy.h"

#include <stdio.h>
#include <stdlib.h>

#include "map.h"

// Pass build order to orders file
void build_order(FILE *file, int id, char type) {
    fprintf(file, "%d B %c\n", id, type);
}

// Pass move order to orders file
void move_order(FILE *file, int id, int x, int y) {
    fprintf(file, "%d M %d %d\n", id, x, y);
}

// Pass attack order to orders file
void attack_order(FILE *file, int id, int enemy_id) {
    fprintf(file, "%d A %d\n", id, enemy_id);
}

// Apply build strategy
void build_strategy(FILE *file, Player *player, Map board) {
    int no_workers, randomIdx, no_units = 7;
    char avalible_units[] = {'K', 'S', 'A', 'P', 'C', 'R', 'W'};
    char affordable_units[no_units];
    int no_affordable = 0;

    // Player is building already
    if (player->base.building != '0') {
        return;
    }
    // Save which units player can afford
    for (int i = 0; i < no_units; i++) {
        if (unit(-1, -1, -1, avalible_units[i]).price < player->gold) {
            affordable_units[no_affordable++] = avalible_units[i];
        }
    }
    // If player can't buy nothing return
    if (no_affordable == 0) {
        return;
    }

    // Build workers if there are mines and player needs them
    if (have_mines(board) && player->gold < 500) {
        // Count how many workers
        for (int i = 0; i < player->no_units; i++) {
            if (player->units[i].type == 'W') {
                no_workers++;
            }
        }
        // If workers are less than desired amount build one
        if (no_workers < MIN_WORKERS) {
            if (player->gold >= unit(-1, -1, -1, 'W').price) {
                build_order(file, player->base.id, 'W');
                return;
            }
        }
    }
    // Generate a random index within the array size
    randomIdx = rand() % no_affordable;
    build_order(file, player->base.id, affordable_units[randomIdx]);
}

// Apply attack strategy
void attack_strategy(FILE *file, Player *player, Player *enemy, Map board) {
    Unit *p_unit, *e_unit;
    int d;
    for (int u = 0; u < player->no_units; u++) {
        p_unit = &(player->units[u]);
        if (p_unit->attacked != 0) {
            // Prioritize enemy base over units
            d = distance(p_unit->x, p_unit->y, enemy->base.x, enemy->base.y);
            if (d <= p_unit->range) {
                attack_order(file, p_unit->id, enemy->base.id);
                p_unit->attacked = 1;
                p_unit->speed--;
            } else {
                // Check if some unit in range and attack it
                for (int eu = 0; eu < enemy->no_units; eu++) {
                    e_unit = &(enemy->units[eu]);
                    d = distance(p_unit->x, p_unit->y, e_unit->x, e_unit->y);

                    if (d <= p_unit->range && e_unit->durability > 0) {
                        attack_order(file, p_unit->id, e_unit->id);
                        e_unit->durability -= get_damage(p_unit->type, e_unit->type);
                        p_unit->attacked = 1;
                        p_unit->speed--;
                        break;
                    }
                }
            }
        }
    }
}

// Apply move strategy
void move_strategy(FILE *file, Player *player, Player *enemy, Map board) {
}