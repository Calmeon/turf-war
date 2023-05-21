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
int distance(int x1, int y1, int x2, int y2);
Unit *getUnitById(Player *player, int id);

#endif
