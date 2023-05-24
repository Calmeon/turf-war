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
void del_unit(Player *p, int idx);

void free_player(Player *p);
int distance(int x1, int y1, int x2, int y2);
Unit *get_unit_by_id(Player *player, int id);

int get_damage(char attacking, char attacked);
int get_dmg_table_idx(char type);

#endif
