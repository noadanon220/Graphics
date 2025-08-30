#include "NPC.h"
#include <iostream>

NPC::NPC(Team t, Role r) : team(t), role(r) {
    static int npcCount = 0;
    if (++npcCount % 10 == 0) { // Log every 10 NPCs
        std::cout << "NPC: created " << Glyph() << " for Team " << (team == TEAM_A ? "A" : "B") << std::endl;
    }
}

NPC::~NPC() {
    static int npcDestructCount = 0;
    if (++npcDestructCount % 10 == 0) { // Log every 10 destructions
        std::cout << "NPC: destroying " << Glyph() << " from Team " << (team == TEAM_A ? "A" : "B") << std::endl;
    }
    delete pCurrentState;
    pCurrentState = nullptr;
}

char NPC::Glyph() const {
    static int glyphCount = 0;
    if (++glyphCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "NPC: Glyph() called for role " << role << std::endl;
    }
    
    switch (role) {
    case CMD: return 'C';
    case WAR: return 'W';
    case MED: return 'M';
    case SUP: return 'P';
    }
    return '?';
}

void NPC::ApplyTeamColor() const {
    static int colorCount = 0;
    if (++colorCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): ApplyTeamColor called" << std::endl;
    }
    
    if (team == TEAM_A) glColor3d(0.85, 0.15, 0.15); // red
    else glColor3d(0.15, 0.35, 0.95); // blue
}

void NPC::Show() const {
    static int showCount = 0;
    if (++showCount % 300 == 0) { // Log every 300 renders
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") 
                  << "): rendering at (" << (int)x << "," << (int)y << ")" << std::endl;
    }
    
    ApplyTeamColor();
    glBegin(GL_QUADS);
    glVertex2d((int)x, (int)y);
    glVertex2d((int)x, (int)y + 1);
    glVertex2d((int)x + 1, (int)y + 1);
    glVertex2d((int)x + 1, (int)y);
    glEnd();
    glColor3d(0, 0, 0);
    double eps = 0.06;
    glBegin(GL_LINE_LOOP);
    glVertex2d((int)x + eps, (int)y + eps);
    glVertex2d((int)x + eps, (int)y + 1 - eps);
    glVertex2d((int)x + 1 - eps, (int)y + 1 - eps);
    glVertex2d((int)x + 1 - eps, (int)y + eps);
    glEnd();
    ApplyTeamColor();
    glRasterPos2d((int)x + 0.35, (int)y + 0.30);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, Glyph());
}

void NPC::setTarget(double tx, double ty) {
    static int setTargetCount = 0;
    if (++setTargetCount % 100 == 0) { // Log every 100 target sets
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): setTarget from (" 
                  << targetX << "," << targetY << ") to (" << tx << "," << ty << ")" << std::endl;
    }
    targetX = tx; targetY = ty;
    RecomputeDirection();
}

void NPC::RecomputeDirection() {
    double dx = targetX - x;
    double dy = targetY - y;
    double len = std::sqrt(dx * dx + dy * dy);
    if (len > 1e-6) {
        dirX = dx / len;
        dirY = dy / len;
        static int recomputeCount = 0;
        if (++recomputeCount % 200 == 0) { // Log every 200 recomputes
            std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): RecomputeDirection to (" 
                      << dirX << "," << dirY << ") length " << len << std::endl;
        }
    }
    else {
        dirX = dirY = 0.0;
        static int zeroDirCount = 0;
        if (++zeroDirCount % 200 == 0) { // Log every 200 zero directions
            std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): RecomputeDirection set to zero (already at target)" << std::endl;
        }
    }
}

void NPC::setCurrentState(State* newState) {
    static int setStateCount = 0;
    if (++setStateCount % 50 == 0) { // Log every 50 state changes
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): setCurrentState to " 
                  << (newState ? "new state" : "null") << std::endl;
    }
    
    if (pCurrentState) {
        pCurrentState->OnExit(this);
        delete pCurrentState;
    }
    pCurrentState = newState;
}

void NPC::SetPathCells(const std::vector<std::pair<int, int>>& cells) {
    path = cells;
    pathIndex = 0;
    // skip if already on first cell
    if (!path.empty() && path[0].first == Row() && path[0].second == Col())
        pathIndex = 1;
    if (pathIndex < path.size()) {
        SetTargetCellCenter(path[pathIndex].first, path[pathIndex].second);
        isMoving = true;
        static int pathCount = 0;
        if (++pathCount % 30 == 0) { // Log every 30 paths
            std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") 
                      << "): new path with " << path.size() << " waypoints, starting at " 
                      << pathIndex << " -> (" << path[pathIndex].first << "," << path[pathIndex].second << ")" << std::endl;
        }
    }
    else {
        isMoving = false;
    }
}

bool NPC::AdvanceWaypoint() {
    if (path.empty()) return false;
    if (pathIndex + 1 < path.size()) {
        ++pathIndex;
        SetTargetCellCenter(path[pathIndex].first, path[pathIndex].second);
        static int waypointCount = 0;
        if (++waypointCount % 50 == 0) { // Log every 50 waypoints
            std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") 
                      << "): advancing to waypoint " << (pathIndex + 1) << "/" << path.size() 
                      << " at (" << path[pathIndex].first << "," << path[pathIndex].second << ")" << std::endl;
        }
        return true;
    }
    return false;
}

void NPC::DoSomeWork() {
    if (isMoving) {
        x += SPEED * dirX;
        y += SPEED * dirY;
        double dx = targetX - x;
        double dy = targetY - y;
        if (dx * dx + dy * dy < 0.25 * 0.25) {
            if (!AdvanceWaypoint() && pCurrentState) {
                static int moveCount = 0;
                if (++moveCount % 100 == 0) { // Log every 100 moves
                    std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") 
                              << "): arrived at waypoint, triggering transition" << std::endl;
                }
                pCurrentState->Transition(this);
            }
        }
    }
}

void NPC::SetXY(double xx, double yy) { 
    static int setXYCount = 0;
    if (++setXYCount % 100 == 0) { // Log every 100 position sets
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): SetXY from (" 
                  << x << "," << y << ") to (" << xx << "," << yy << ")" << std::endl;
    }
    x = xx; y = yy; 
}

void NPC::setIsMoving(bool v) { 
    static int setIsMovingCount = 0;
    if (++setIsMovingCount % 100 == 0) { // Log every 100 movement state changes
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): setIsMoving from " 
                  << (isMoving ? "true" : "false") << " to " << (v ? "true" : "false") << std::endl;
    }
    isMoving = v; 
}

// Set target to the centre of cell (r,c)
void NPC::SetTargetCellCenter(int r, int c) { 
    static int setTargetCellCount = 0;
    if (++setTargetCellCount % 100 == 0) { // Log every 100 cell target sets
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): SetTargetCellCenter to (" 
                  << r << "," << c << ") center (" << (c + 0.5) << "," << (r + 0.5) << ")" << std::endl;
    }
    setTarget(c + 0.5, r + 0.5); 
}

// Environment (for A*)
void NPC::AttachEnvironment(int map[MSZ][MSZ], double smap[MSZ][MSZ]) {
    static int attachEnvCount = 0;
    if (++attachEnvCount % 50 == 0) { // Log every 50 environment attachments
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): AttachEnvironment maps" << std::endl;
    }
    envMap = map; envSMap = smap;
}

int    (*NPC::EnvMap())[MSZ] { 
    static int envMapCount = 0;
    if (++envMapCount % 500 == 0) { // Log every 500 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): EnvMap called" << std::endl;
    }
    return envMap; 
}

double (*NPC::EnvSMap())[MSZ] { 
    static int envSMapCount = 0;
    if (++envSMapCount % 500 == 0) { // Log every 500 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): EnvSMap called" << std::endl;
    }
    return envSMap; 
}

int NPC::Row() const { 
    static int rowCount = 0;
    if (++rowCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): Row() called, returning " << (int)y << std::endl;
    }
    return (int)y; 
}

int NPC::Col() const { 
    static int colCount = 0;
    if (++colCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): Col() called, returning " << (int)x << std::endl;
    }
    return (int)x; 
}

double NPC::X() const { 
    static int xCount = 0;
    if (++xCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): X() called, returning " << x << std::endl;
    }
    return x; 
}

double NPC::Y() const { 
    static int yCount = 0;
    if (++yCount % 1000 == 0) { // Log every 1000 calls
        std::cout << "NPC " << Glyph() << " (" << (team == TEAM_A ? "A" : "B") << "): Y() called, returning " << y << std::endl;
    }
    return y; 
}
