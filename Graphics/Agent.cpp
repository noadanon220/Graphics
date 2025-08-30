#include "Agent.h"

// Constructor
Agent::Agent(Team t, Role r) : team(t), role(r) {
    static int agentCount = 0;
    if (++agentCount % 10 == 0) { // Log every 10 agents
        std::cout << "AGENT: created " << Glyph() << " for Team " << (team == TEAM_A ? "A" : "B") << std::endl;
    }
}

void Agent::SetPosition(int row, int col) {
    static int setPosCount = 0;
    if (++setPosCount % 100 == 0) { // Log every 100 position sets
        std::cout << "AGENT " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): SetPosition to (" << row << "," << col << ")" << std::endl;
    }
    r = row;
    c = col;
}

char Agent::Glyph() const {
    static int glyphCount = 0;
    if (++glyphCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "AGENT: Glyph() called for role " << role << std::endl;
    }
    
    switch (role) {
    case CMD: return 'C';
    case WAR: return 'W';
    case MED: return 'M';
    case SUP: return 'P';
    }
    return '?';
}

void Agent::ApplyTeamColor() const {
    static int colorCount = 0;
    if (++colorCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "AGENT " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): ApplyTeamColor called" << std::endl;
    }
    
    // Team A: red, Team B: blue
    if (team == TEAM_A) glColor3d(0.85, 0.15, 0.15);
    else                glColor3d(0.15, 0.35, 0.95);
}

void Agent::Draw() const {
    static int drawCount = 0;
    if (++drawCount % 500 == 0) { // Log every 500 renders
        std::cout << "AGENT " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): Draw called at (" << r << "," << c << ")" << std::endl;
    }
    
    // Fill 1x1 cell by team color
    ApplyTeamColor();
    glBegin(GL_QUADS);
    glVertex2d(c, r);
    glVertex2d(c, r + 1);
    glVertex2d(c + 1, r + 1);
    glVertex2d(c + 1, r);
    glEnd();

    // Thin black stroke
    glColor3d(0, 0, 0);
    const double eps = 0.06;
    glBegin(GL_LINE_LOOP);
    glVertex2d(c + eps, r + eps);
    glVertex2d(c + eps, r + 1 - eps);
    glVertex2d(c + 1 - eps, r + 1 - eps);
    glVertex2d(c + 1 - eps, r + eps);
    glEnd();

    // Glyph in team color
    ApplyTeamColor();
    glRasterPos2d(c + 0.35, r + 0.30);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Glyph());
}
