// ============== GameMap.cpp ==============
#include "GameMap.h"
#include <cstdio>
#include "glut.h"

GameMap::GameMap() {
    Initialize();
}

void GameMap::Initialize() {
    // Initialize entire map as empty space
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            terrain[i][j] = TerrainType::SPACE;
        }
    }

    // Add random rocks
    for (int i = 0; i < 60; i++) {
        int x = rand() % (MAP_SIZE - 4) + 2;
        int y = rand() % (MAP_SIZE - 4) + 2;
        terrain[y][x] = TerrainType::ROCK;
    }

    // Add random trees
    for (int i = 0; i < 40; i++) {
        int x = rand() % (MAP_SIZE - 4) + 2;
        int y = rand() % (MAP_SIZE - 4) + 2;
        terrain[y][x] = TerrainType::TREE;
    }

    // Add some water areas
    for (int i = 0; i < 20; i++) {
        int x = rand() % (MAP_SIZE - 4) + 2;
        int y = rand() % (MAP_SIZE - 4) + 2;
        terrain[y][x] = TerrainType::WATER;
    }

    // Fixed supply depot locations
    // Team 1 (blue) supplies - bottom left area
    terrain[3][3] = TerrainType::MED_SUPPLY;
    terrain[3][7] = TerrainType::AMMO_SUPPLY;

    // Team 2 (orange) supplies - top right area
    terrain[MAP_SIZE - 4][MAP_SIZE - 4] = TerrainType::MED_SUPPLY;
    terrain[MAP_SIZE - 4][MAP_SIZE - 8] = TerrainType::AMMO_SUPPLY;
}

void GameMap::Show() {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            // Set color based on terrain type
            switch (terrain[i][j]) {
            case TerrainType::SPACE:
                glColor3d(0.4, 0.8, 0.4); // light green
                break;
            case TerrainType::ROCK:
                glColor3d(0.3, 0.3, 0.3); // dark gray
                break;
            case TerrainType::TREE:
                glColor3d(0.1, 0.5, 0.1); // dark green
                break;
            case TerrainType::WATER:
                glColor3d(0.2, 0.4, 0.8); // blue
                break;
            case TerrainType::MED_SUPPLY:
                glColor3d(1.0, 1.0, 0.0); // yellow
                break;
            case TerrainType::AMMO_SUPPLY:
                glColor3d(1.0, 0.8, 0.0); // darker yellow
                break;
            }

            // Draw cell
            glBegin(GL_POLYGON);
            glVertex2d(j, i);
            glVertex2d(j + 1, i);
            glVertex2d(j + 1, i + 1);
            glVertex2d(j, i + 1);
            glEnd();

            // Draw border
            glColor3d(0.2, 0.2, 0.2);
            glLineWidth(1);
            glBegin(GL_LINE_LOOP);
            glVertex2d(j, i);
            glVertex2d(j + 1, i);
            glVertex2d(j + 1, i + 1);
            glVertex2d(j, i + 1);
            glEnd();

            // Draw supply depot symbols
            if (terrain[i][j] == TerrainType::MED_SUPPLY) {
                glColor3d(1, 0, 0); // red M
                glRasterPos2d(j + 0.3, i + 0.6);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'M');
            }
            else if (terrain[i][j] == TerrainType::AMMO_SUPPLY) {
                glColor3d(0, 0, 0); // black A
                glRasterPos2d(j + 0.3, i + 0.6);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
            }
        }
    }
}

TerrainType GameMap::getTerrain(int x, int y) {
    if (!isInBounds(x, y)) return TerrainType::ROCK;
    return terrain[y][x];
}

void GameMap::setTerrain(int x, int y, TerrainType type) {
    if (isInBounds(x, y)) {
        terrain[y][x] = type;
    }
}

bool GameMap::isPassable(int x, int y) {
    if (!isInBounds(x, y)) return false;
    TerrainType t = getTerrain(x, y);
    return (t != TerrainType::ROCK && t != TerrainType::WATER);
}

bool GameMap::blocksVision(int x, int y) {
    if (!isInBounds(x, y)) return true;
    TerrainType t = getTerrain(x, y);
    return (t == TerrainType::ROCK || t == TerrainType::TREE);
}

bool GameMap::isInBounds(int x, int y) {
    return (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE);
}