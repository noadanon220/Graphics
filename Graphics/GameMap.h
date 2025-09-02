// ============== GameMap.h ==============
#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <cstdlib>    // Add this for rand
#include <cstdio>     // Add this for sprintf

const int MAP_SIZE = 40;

enum class TerrainType {
    SPACE = 0,
    ROCK = 1,
    TREE = 2,
    WATER = 3,
    MED_SUPPLY = 4,
    AMMO_SUPPLY = 5
};

class GameMap {
private:
    TerrainType terrain[MAP_SIZE][MAP_SIZE];

public:
    GameMap();
    void Initialize();
    void Show();

    TerrainType getTerrain(int x, int y);
    void setTerrain(int x, int y, TerrainType type);
    bool isPassable(int x, int y);
    bool blocksVision(int x, int y);
    bool isInBounds(int x, int y);
};

#endif