// Definitions.h
#pragma once


const int MSZ = 100;

// cell size in screen pixels
const int CELL_SIZE = 10;

// terrain
const int SPACE = 0;
const int STONE = 1;      
const int TREE = 2;
const int WATER = 3;
const int DEPOT_AMMO = 4;
const int DEPOT_MED = 5;

// physics / risk
const double SPEED = 0.1;
const double SECURITY_VALUE = 0.01;

// terrain rules
inline bool BlocksMovement(int cell) { return cell == STONE || cell == WATER; } // trees are passable
inline bool BlocksFire(int cell) { return cell == STONE || cell == TREE; } // water doesn't block fire
inline bool BlocksVision(int cell) { return cell == STONE || cell == TREE; } // water doesn't block vision

// Teams and Roles
enum Team { TEAM_A = 0, TEAM_B = 1 };
enum Role { CMD, WAR, MED, SUP }; // Commander, Warrior, Medic, Supplier