#ifndef UNITS_H
#define UNITS_H

// Struct representing player base
typedef struct {
    int id, x, y, durability, building_duration;
    char building;
} Base;

// Unit representing game unit
typedef struct {
    int id, x, y, durability, speed, price, range, build_time, attacked;
    char type;
} Unit;

// Constructors for different units

Base base(int id, int x, int y, int durability, char building, int building_duration);
Unit knight();
Unit swordsman();
Unit archer();
Unit pikeman();
Unit ram();
Unit catapult();
Unit worker();

Unit unit(int id, int x, int y, char type);

#endif
