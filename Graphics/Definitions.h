//Definitions.h
#pragma once

// Board size and drawing scale
const int MSZ = 100;
const int CELL_SIZE = 10;

// Terrain types
const int SPACE = 0;
const int STONE = 1;
const int TREE = 2;
const int WATER = 3;
const int DEPOT_AMMO = 4;
const int DEPOT_MED = 5;

// Physics / risk constants
const double SPEED = 0.1;            // Movement speed per frame
const double SECURITY_VALUE = 0.01;  // Risk increment per bullet step

// Terrain rules
inline bool BlocksMovement(int cell) { return cell == STONE || cell == WATER; }
inline bool BlocksFire(int cell) { return cell == STONE || cell == TREE; }
inline bool BlocksVision(int cell) { return cell == STONE || cell == TREE; }

// Teams and roles
enum Team { TEAM_A = 0, TEAM_B = 1 };
enum Role { CMD, WAR, MED, SUP };

// Combat constants
const int BULLET_DAMAGE = 20;
const int GRENADE_DAMAGE = 60;
const int MAX_AMMO = 20;

// Warrior fitness thresholds (interpreting spec):
// >=50% HP -> can fight; 25%-49% -> can move only; <25% -> not fit (practically disabled).
const int WARRIOR_FIGHT_HP_PCT = 50;
const int WARRIOR_MOVE_ONLY_HP_PCT = 25;
