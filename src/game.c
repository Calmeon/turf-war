// File with shared game functions

#include "game.h"

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
Unit *getUnitById(Player *player, int id) {
    for (int i = 0; i < player->no_units; i++) {
        if (player->units[i].id == id) {
            return &(player->units[i]);
        }
    }
    return NULL;
}
