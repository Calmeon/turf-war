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

// Check if some enemy is in range of unit
int enemy_in_range(Unit unit, Player enemy) {
    int d = distance(unit.x, unit.y, enemy.base.x, enemy.base.y);
    if (d <= unit.range) {
        return 1;
    }
    for (int i = 0; i < enemy.no_units; i++) {
        d = distance(unit.x, unit.y, enemy.units[i].x, enemy.units[i].y);
        if (d <= unit.range) {
            return 1;
        }
    }
    return 0;
}

// Check if enemy/enemy base on specific (x,y)
int enemy_on_xy(Player enemy, int x, int y) {
    if (enemy.base.x == x && enemy.base.y == y) {
        return 1;
    }
    for (int i = 0; i < enemy.no_units; i++) {
        if (enemy.units[i].x == x && enemy.units[i].y == y) {
            return 1;
        }
    }
    return 0;
}

// Apply build strategy
void build_strategy(FILE *file, Player *player, Map board) {
    int no_workers, random_idx, no_units = 7;
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
    random_idx = rand() % no_affordable;
    build_order(file, player->base.id, affordable_units[random_idx]);
}

// Apply attack strategy
void attack_strategy(FILE *file, Player *player, Player *enemy) {
    Unit *p_unit, *e_unit;
    int d;
    for (int u = 0; u < player->no_units; u++) {
        p_unit = &(player->units[u]);
        if (p_unit->attacked == 0) {
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
                        if (e_unit->durability <= 0)
                            del_unit(enemy, e_unit->id);
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
    Unit *p_unit;
    int possible_x[64], possible_y[64], good_x[64], good_y[64];
    int v, new_x, new_y, d, random_idx, no_possible, no_good, best_d;

    for (int i = 0; i < player->no_units; i++) {
        p_unit = &(player->units[i]);
        no_possible = 0, no_good = 0;
        // Don't move if enemy in range
        if (!enemy_in_range(*p_unit, *enemy)) {
            v = p_unit->speed;
            // Go through possible coords to move and filter them
            for (int x = -v; x <= v; x++) {
                for (int y = -v; y <= v; y++) {
                    new_x = p_unit->x + x;
                    new_y = p_unit->y + y;

                    // Stay in place
                    if (new_x == p_unit->x && new_y == p_unit->y)
                        continue;
                    // Not enough speed
                    if (abs(x) + abs(y) > v)
                        continue;
                    // Point beyond the map
                    if (new_x >= board.no_cols || new_x < 0 || new_y >= board.no_rows || new_y < 0)
                        continue;
                    // Obstacle on coordinat
                    if (board.board_matrix[new_y][new_x] == '9')
                        continue;
                    // Enemy in range
                    if (enemy_on_xy(*enemy, new_x, new_y))
                        continue;

                    // Good move
                    possible_x[no_possible] = new_x;
                    possible_y[no_possible++] = new_y;
                }
            }
            // Pick best moves

            // Check if there are moves that allow move and attack
            if (p_unit->attacked == 0) {
                for (int i = 0; i < no_possible; i++) {
                    if (enemy_in_range(unit(-1, possible_x[i], possible_y[i], p_unit->type), *enemy) &&
                        distance(p_unit->x, p_unit->y, possible_x[i], possible_y[i]) < p_unit->speed) {
                        good_x[no_good] = possible_x[i];
                        good_y[no_good++] = possible_y[i];
                    }
                }
            }
            // Get only the furthest moves if can't attack
            if (no_good == 0) {
                // Check best distance
                best_d = distance(p_unit->x, p_unit->y, enemy->base.x, enemy->base.y);
                for (int i = 0; i < no_possible; i++) {
                    d = distance(possible_x[i], possible_y[i], enemy->base.x, enemy->base.y);
                    if (d < best_d)
                        best_d = d;
                }
                // Add furthest moves to possibilities
                for (int i = 0; i < no_possible; i++) {
                    d = distance(possible_x[i], possible_y[i], enemy->base.x, enemy->base.y);
                    if (d == best_d) {
                        good_x[no_good] = possible_x[i];
                        good_y[no_good++] = possible_y[i];
                    }
                }
            }

            // If there are good moves draw one and do it
            if (no_good == 0)
                continue;

            random_idx = rand() % no_good;
            p_unit->speed -= distance(p_unit->x, p_unit->y, good_x[random_idx], good_y[random_idx]);
            move_order(file, p_unit->id, good_x[random_idx], good_y[random_idx]);
        }
    }
}
