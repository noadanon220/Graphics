#pragma once

const int MSZ = 100;       // Map size
const int CELL_SIZE = 10;  // Pixel size (unused in this logic)

// Terrain types
const int SPACE = 0;
const int STONE = 1;
const int TREE = 2;
const int WATER = 3;
const int DEPOT_AMMO = 4;
const int DEPOT_MED = 5;

// Physics / risk constants
const double SPEED = 0.1;         // Movement speed per frame
const double SECURITY_VALUE = 0.01; // Risk increment per bullet step

// Terrain rules
inline bool BlocksMovement(int cell) { return cell == STONE || cell == WATER; }
inline bool BlocksFire(int cell) { return cell == STONE || cell == TREE; }
inline bool BlocksVision(int cell) { return cell == STONE || cell == TREE; }

// Teams and roles
enum Team { TEAM_A = 0, TEAM_B = 1 };
enum Role { CMD, WAR, MED, SUP };
