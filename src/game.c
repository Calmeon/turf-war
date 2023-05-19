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
