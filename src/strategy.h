#ifndef STRATEGY_H
#define STRATEGY_H

#include <stdio.h>

#include "game.h"
#include "map.h"

#define MIN_WORKERS 3

void build_order(FILE *file, int id, char type);
void move_order(FILE *file, int id, int x, int y);
void attack_order(FILE *file, int id, int enemy_id);

int enemy_in_range(Unit unit, Player enemy);
int enemy_on_xy(Player enemy, int x, int y);

void build_strategy(FILE *file, Player *player, Map board);
void attack_strategy(FILE *file, Player *player, Player *enemy);
void move_strategy(FILE *file, Player *player, Player *enemy, Map board);

#endif
