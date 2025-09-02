// ============== Character.cpp ==============
#include "Character.h"
#include "IdleState.h"
#include <cmath>
#include <algorithm>
#include <queue>

const double CHARACTER_SPEED = 0.05;

Character::Character(Position p, int tm, CharacterType tp) {
    pos = p;
    team = tm;
    type = tp;
    health = 100;
    maxHealth = 100;
    alive = true;
    isMoving = false;
    currentState = new IdleState();

    // Initialize visibility map
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            visibilityMap[i][j] = false;
        }
    }
}

Character::~Character() {
    delete currentState;
}

void Character::MoveTo(Position newTarget) {
    target = newTarget;
    isMoving = true;

    // Calculate direction
    double distance = Distance(pos, target);
    if (distance > 0.1) {
        dirX = (target.x - pos.x) / distance;
        dirY = (target.y - pos.y) / distance;
    }
}

void Character::UpdateMovement() {
    if (!isMoving) return;

    double distance = Distance(pos, target);
    if (distance < 0.1) {
        pos = target;
        isMoving = false;
        return;
    }

    pos.x += CHARACTER_SPEED * dirX;
    pos.y += CHARACTER_SPEED * dirY;
}

void Character::UpdateVisibilityMap(GameMap* map) {
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            visibilityMap[i][j] = CanSee(Position(j + 0.5, i + 0.5), map);
        }
    }
}

bool Character::CanSee(Position target, GameMap* map) {
    // Simple line of sight using Bresenham-like algorithm
    double dx = target.x - pos.x;
    double dy = target.y - pos.y;
    double distance = sqrt(dx * dx + dy * dy);

    // Maximum vision range
    if (distance > 15.0) return false;

    // Check line of sight
    int steps = (int)(distance * 2);
    if (steps == 0) return true;

    double stepX = dx / steps;
    double stepY = dy / steps;

    for (int i = 1; i < steps; i++) {
        double checkX = pos.x + i * stepX;
        double checkY = pos.y + i * stepY;

        if (map->blocksVision((int)checkX, (int)checkY)) {
            return false;
        }
    }

    return true;
}

std::vector<Position> Character::FindPath(Position goal, GameMap* map) {
    // Simple A* pathfinding implementation
    std::vector<Position> path;

    int startX = (int)pos.x;
    int startY = (int)pos.y;
    int goalX = (int)goal.x;
    int goalY = (int)goal.y;

    // For simplicity, use direct path if possible
    if (map->isPassable(goalX, goalY)) {
        path.push_back(Position(goalX + 0.5, goalY + 0.5));
    }

    return path;
}

void Character::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        alive = false;
    }
}

void Character::heal(int amount) {
    health = std::min(maxHealth, health + amount);
}

void Character::setState(State* newState) {
    if (currentState) {
        currentState->OnExit(this);
        delete currentState;
    }
    currentState = newState;
    currentState->OnEnter(this);
}
