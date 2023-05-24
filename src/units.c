// Units constructors and functions

#include "units.h"

#include <stdio.h>

Base base(int id, int x, int y, int durability, char building, int building_duration) {
    Base b = {.id = id, .x = x, .y = y, .durability = durability, .building = building, .building_duration = building_duration};
    return b;
}

Unit knight() {
    Unit k = {.type = 'K', .durability = 70, .speed = 5, .price = 400, .range = 1, .build_time = 5};
    return k;
}

Unit swordsman() {
    Unit s = {.type = 'S', .durability = 60, .speed = 2, .price = 250, .range = 1, .build_time = 3};
    return s;
}

Unit archer() {
    Unit a = {.type = 'A', .durability = 40, .speed = 2, .price = 250, .range = 5, .build_time = 3};
    return a;
}

Unit pikeman() {
    Unit p = {.type = 'P', .durability = 50, .speed = 2, .price = 200, .range = 2, .build_time = 3};
    return p;
}

Unit ram() {
    Unit r = {.type = 'R', .durability = 90, .speed = 2, .price = 500, .range = 1, .build_time = 4};
    return r;
}

Unit catapult() {
    Unit c = {.type = 'C', .durability = 50, .speed = 2, .price = 800, .range = 7, .build_time = 6};
    return c;
}

Unit worker() {
    Unit w = {.type = 'W', .durability = 20, .speed = 2, .price = 100, .range = 1, .build_time = 2};
    return w;
}

// Unit constructor which returns type of specified unit
Unit unit(int id, int x, int y, char type) {
    Unit u;

    // Check type of unit and create it
    switch (type) {
        case 'K':
            u = knight();
            break;
        case 'S':
            u = swordsman();
            break;
        case 'A':
            u = archer();
            break;
        case 'P':
            u = pikeman();
            break;
        case 'R':
            u = ram();
            break;
        case 'C':
            u = catapult();
            break;
        case 'W':
            u = worker();
            break;
        default:
            printf("Unknown type\n");
            break;
    }

    u.id = id;
    u.x = x;
    u.y = y;
    u.attacked = 0;

    return u;
}
