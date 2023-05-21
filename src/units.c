// Units constructors and functions

#include "units.h"

#include <stdio.h>

Base base(int id, int x, int y, int durability, char building, int building_duration) {
    Base b = {.id = id, .x = x, .y = y, .durability = durability, .building = building, .building_duration = building_duration};
    return b;
}

Unit knight(int id, int x, int y) {
    Unit k = {.id = id, .x = x, .y = y, .type = 'K', .durability = 70, .speed = 5, .price = 400, .range = 1, .build_time = 5};
    return k;
}

Unit swordsman(int id, int x, int y) {
    Unit s = {.id = id, .x = x, .y = y, .type = 'S', .durability = 60, .speed = 2, .price = 250, .range = 1, .build_time = 3};
    return s;
}

Unit archer(int id, int x, int y) {
    Unit a = {.id = id, .x = x, .y = y, .type = 'A', .durability = 40, .speed = 2, .price = 250, .range = 5, .build_time = 3};
    return a;
}

Unit pikeman(int id, int x, int y) {
    Unit p = {.id = id, .x = x, .y = y, .type = 'P', .durability = 50, .speed = 2, .price = 200, .range = 2, .build_time = 3};
    return p;
}

Unit ram(int id, int x, int y) {
    Unit r = {.id = id, .x = x, .y = y, .type = 'R', .durability = 90, .speed = 2, .price = 500, .range = 1, .build_time = 4};
    return r;
}

Unit catapult(int id, int x, int y) {
    Unit c = {.id = id, .x = x, .y = y, .type = 'C', .durability = 50, .speed = 2, .price = 800, .range = 7, .build_time = 6};
    return c;
}

Unit worker(int id, int x, int y) {
    Unit w = {.id = id, .x = x, .y = y, .type = 'W', .durability = 20, .speed = 2, .price = 100, .range = 1, .build_time = 2};
    return w;
}

Unit unit(int id, int x, int y, char type) {
    Unit u;

    // Check type of unit and create it
    switch (type) {
        case 'K':
            u = knight(id, x, y);
            break;
        case 'S':
            u = swordsman(id, x, y);
            break;
        case 'A':
            u = archer(id, x, y);
            break;
        case 'P':
            u = pikeman(id, x, y);
            break;
        case 'R':
            u = ram(id, x, y);
            break;
        case 'C':
            u = catapult(id, x, y);
            break;
        case 'W':
            u = worker(id, x, y);
            break;
        default:
            printf("Unknown type\n");
            break;
    }

    return u;
}
