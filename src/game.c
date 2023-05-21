// File with shared game functions

#include "game.h"

#include <stdio.h>
#include <stdlib.h>

// Add unit to player units with proper memory reallocation
void add_unit(Player *p, Unit u) {
    p->no_units++;
    if (p->no_units == 1) {
        p->units = malloc(p->no_units * sizeof(Unit));
    } else {
        p->units = realloc(p->units, p->no_units * sizeof(Unit));
    }
    p->units[p->no_units - 1] = u;
}

// Free units memory of player
void free_player(Player *p) {
    if (p->units != NULL) {
        free(p->units);
    }
}

// Calculate distance beetwen two points
int distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Return unit with given id
Unit *get_unit_by_id(Player *player, int id) {
    for (int i = 0; i < player->no_units; i++) {
        if (player->units[i].id == id) {
            return &(player->units[i]);
        }
    }
    return NULL;
}

// Get idx of dmg table where unit have dmg info
int get_dmg_table_idx(char type) {
    switch (type) {
        case 'K':
            return 0;
        case 'S':
            return 1;
        case 'A':
            return 2;
        case 'P':
            return 3;
        case 'C':
            return 4;
        case 'R':
            return 5;
        case 'W':
            return 6;
        case 'B':
            return 7;
        default:
            printf("Unknown type\n");
            break;
    }
    return 0;
}

// From dmg table get adequate dmg value
int get_damage(char attacking, char attacked) {
    int attackerIdx, attackedIdx;
    // Damage table for units
    int damage_table[7][8] = {
        // Attacker 0  Attacker 1  Attacker 2  Attacker 3  Attacker 4  Attacker 5  Attacker 6  Attacker 7
        {35, 35, 35, 35, 35, 50, 35, 35},  // Target 0
        {30, 30, 30, 20, 20, 30, 30, 30},  // Target 1
        {15, 15, 15, 15, 10, 10, 15, 15},  // Target 2
        {35, 15, 15, 15, 15, 10, 15, 10},  // Target 3
        {40, 40, 40, 40, 40, 40, 40, 50},  // Target 4
        {10, 10, 10, 10, 10, 10, 10, 50},  // Target 5
        {5, 5, 5, 5, 5, 5, 5, 1}           // Target 6
    };

    attackerIdx = get_dmg_table_idx(attacking);
    attackedIdx = get_dmg_table_idx(attacked);

    return damage_table[attackerIdx][attackedIdx];
}