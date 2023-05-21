#ifndef UNITS_H
#define UNITS_H

// Struct representing player base
typedef struct {
    int id, x, y, durability, building_duration;
    char building;
} Base;

// Unit representing game unit
typedef struct {
    int id, x, y, durability, speed, price, range, build_time;
    char type;
} Unit;

// Constructors for different units

Base base(int id, int x, int y, int durability, char building, int building_duration);
Unit knight(int id, int x, int y);
Unit swordsman(int id, int x, int y);
Unit archer(int id, int x, int y);
Unit pikeman(int id, int x, int y);
Unit ram(int id, int x, int y);
Unit catapult(int id, int x, int y);
Unit worker(int id, int x, int y);
// Unit constructor which returns type of specified unit
Unit unit(int id, int x, int y, char type);

#endif
