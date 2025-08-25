#pragma once
#include "Definitions.h"
#include "glut.h"

// Represents a single player unit on the grid.
class Agent {
public:
    Agent(Team team, Role role);

    // Grid position (row/col)
    int r = 0;
    int c = 0;

    // Identity
    Team team;
    Role role;

    // Behavior hooks (placeholders for future AI)
    void SetPosition(int row, int col);
    void Draw() const; // draw 1x1 cell with letter

private:
    char Glyph() const;        // 'C','W','M','P'
    void ApplyTeamColor() const; // red for A, blue for B
};
