#pragma once
#include <vector>
#include <utility>
#include <cmath>
#include "Definitions.h"
#include "State.h"
#include "glut.h"

// Base NPC: holds position, path, state machine, environment pointers
class NPC {
public:
    NPC(Team t, Role r);
    virtual ~NPC();

    // Identity
    Team team;
    Role role;

    // Rendering (square + glyph)
    virtual void Show() const;

    // Per-frame logic: move along current path and trigger transitions
    virtual void DoSomeWork();

    // Position
    void SetXY(double xx, double yy);
    double X() const;
    double Y() const;
    int Row() const;
    int Col() const;

    // Movement
    void setIsMoving(bool v);
    void setTarget(double tx, double ty);
    void setCurrentState(State* newState);
    State* getCurrentState() { return pCurrentState; }

    // Environment (for A*)
    void AttachEnvironment(int map[MSZ][MSZ], double smap[MSZ][MSZ]);
    int    (*EnvMap())[MSZ];
    double (*EnvSMap())[MSZ];

    // Path following
    void SetPathCells(const std::vector<std::pair<int, int>>& cells);
    bool AdvanceWaypoint();

protected:
    // Glyph char by role
    char Glyph() const;
    // Team colour
    void ApplyTeamColor() const;
    // Recompute movement direction
    void RecomputeDirection();
    // Set target to the centre of cell (r,c)
    void SetTargetCellCenter(int r, int c);

private:
    // Continuous position
    double x = 0.5, y = 0.5;
    // Movement target
    double targetX = 0.5, targetY = 0.5;
    // Direction vectors
    double dirX = 0.0, dirY = 0.0;
    // Is currently moving
    bool isMoving = false;
    // State pointer (owned)
    State* pCurrentState = nullptr;
    // Environment pointers (not owned)
    int    (*envMap)[MSZ] = nullptr;
    double (*envSMap)[MSZ] = nullptr;
    // Waypoints
    std::vector<std::pair<int, int>> path;
    size_t pathIndex = 0;
};
