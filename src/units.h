#ifndef UNITS_H
#define UNITS_H

// Struct representing player base
typedef struct {
    int id, x, y, durability;
    char building;
} Base;

// Unit representing game unit
typedef struct {
    int id, x, y, durability, speed, price, range, build_time;
    char type;
} Unit;

// Constructors for different units

Unit knight(int id, int x, int y);
Unit swordsman(int id, int x, int y);
Unit archer(int id, int x, int y);
Unit pikeman(int id, int x, int y);
Unit ram(int id, int x, int y);
Unit catapult(int id, int x, int y);
Unit worker(int id, int x, int y);

#endif
