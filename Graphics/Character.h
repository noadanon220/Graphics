// ============== Character.h ==============
#ifndef CHARACTER_H
#define CHARACTER_H

#include "Position.h"
#include "State.h" 
#include "GameMap.h"
#include <vector>
#include <cmath>      // Add this for sqrt
#include <cstdlib>    // Add this for abs
#include <algorithm>  // Add this for min/max

enum class CharacterType {
    COMMANDER = 'C',
    WARRIOR = 'W',
    MEDIC = 'M',
    SUPPLIER = 'P'
};

class Character {
protected:
    Position pos;
    Position target;
    int health;
    int maxHealth;
    int team;
    CharacterType type;
    State* currentState;
    bool alive;
    bool isMoving;
    double dirX, dirY;
    bool visibilityMap[MAP_SIZE][MAP_SIZE];

public:
    Character(Position p, int tm, CharacterType tp);
    virtual ~Character();

    virtual void Update(GameMap* map) = 0;
    virtual void Show() = 0;

    // Core functionality
    void MoveTo(Position target);
    void UpdateMovement();
    void UpdateVisibilityMap(GameMap* map);
    bool CanSee(Position target, GameMap* map);
    std::vector<Position> FindPath(Position goal, GameMap* map);

    // Getters
    Position getPosition() const { return pos; }
    int getHealth() const { return health; }
    int getTeam() const { return team; }
    CharacterType getType() const { return type; }
    bool isAlive() const { return alive; }
    bool getVisibility(int x, int y) const { return visibilityMap[y][x]; }

    // Setters
    void takeDamage(int damage);
    void heal(int amount);
    void setState(State* newState);
};

#endif