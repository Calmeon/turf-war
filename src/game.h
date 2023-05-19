#ifndef GAME_H
#define GAME_H

#include "units.h"

// Player structure
typedef struct {
    int gold, no_units;
    Base base;
    Unit *units;
} Player;

void add_unit(Player *p, Unit u);
void free_player(Player *p);

#endif
