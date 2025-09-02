// ============== Projectile.h ==============
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Position.h"
#include "GameMap.h"
#include <GL/glut.h>

enum class ProjectileType {
    BULLET,
    GRENADE
};

class Projectile {
private:
    Position start;
    Position target;
    Position current;
    Position direction;
    ProjectileType type;
    int damage;
    int team;
    double speed;
    bool active;
    double radius; // For grenade explosion

public:
    Projectile(Position s, Position t, ProjectileType pt, int dmg, int tm);

    void Update(GameMap* map);
    void Show();
    bool isActive() const { return active; }
    Position getCurrentPos() const { return current; }
    int getDamage() const { return damage; }
    int getTeam() const { return team; }
    ProjectileType getType() const { return type; }
    double getRadius() const { return radius; }
};

#endif
