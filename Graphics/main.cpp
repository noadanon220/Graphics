#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <unordered_set>
#include <cassert>
#include <algorithm>
#include <iostream>
#include "glut.h"

#include "Definitions.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Team.h"
#include "MedicNPC.h"
#include "SupplierNPC.h"
#include "WarriorNPC.h"

// Terrain counts
const int NUM_STONE = 200;
const int NUM_WATER = 50;
const int NUM_TREES = 100;

// Window size
const int W = 700, H = 700;

// Global terrain and risk map
int    map[MSZ][MSZ] = { 0 };
double smap[MSZ][MSZ] = { 0.0 };
bool   showSecurityMap = false;

// Teams
TeamSquad g_teamA(TEAM_A);
TeamSquad g_teamB(TEAM_B);

// Depots
std::vector<std::pair<int, int>> g_ammoDepots[2];
std::vector<std::pair<int, int>> g_medDepots[2];

inline int CellKey(int r, int c) { return r * MSZ + c; }
std::unordered_set<int> g_cellsTeamA, g_cellsTeamB;

Grenade* pg = nullptr;

// Register depot and record its owner
static void RegisterDepotCell(Team team, int r, int c, bool isAmmo) {
    std::cout << "SETUP: registering " << (isAmmo ? "ammo" : "med") << " depot for Team " 
              << (team == TEAM_A ? "A" : "B") << " at (" << r << "," << c << ")" << std::endl;
    if (team == TEAM_A) g_cellsTeamA.insert(CellKey(r, c));
    else g_cellsTeamB.insert(CellKey(r, c));
    if (isAmmo) g_ammoDepots[team].push_back({ r,c });
    else g_medDepots[team].push_back({ r,c });
}

// Random scatter function
static void PlaceRandomCells(int m[MSZ][MSZ], int cellValue, int count) {
    std::cout << "SETUP: placing " << count << " cells of type " << cellValue << std::endl;
    int placed = 0;
    while (placed < count) {
        int r = rand() % MSZ;
        int c = rand() % MSZ;
        if (m[r][c] == SPACE) {
            m[r][c] = cellValue;
            placed++;
        }
    }
    std::cout << "SETUP: placed " << placed << " cells of type " << cellValue << std::endl;
}

// Place depot in subregion
static void PlaceDepotInRegion(int m[MSZ][MSZ], int depotType,
    int r0, int r1, int c0, int c1)
{
    std::cout << "SETUP: placing depot type " << depotType << " in region (" << r0 << "," << c0 << ") to (" << r1 << "," << c1 << ")" << std::endl;
    const int margin = 2;
    r0 = std::max(r0 + margin, 0);
    c0 = std::max(c0 + margin, 0);
    r1 = std::min(r1 - margin, MSZ - 1);
    c1 = std::min(c1 - margin, MSZ - 1);
    for (int tries = 0; tries < 1000; ++tries) {
        int r = r0 + rand() % (r1 - r0 + 1);
        int c = c0 + rand() % (c1 - c0 + 1);
        if (m[r][c] == SPACE) { 
            m[r][c] = depotType; 
            std::cout << "SETUP: depot type " << depotType << " placed at (" << r << "," << c << ")" << std::endl;
            return; 
        }
    }
    for (int r = r0; r <= r1; ++r)
        for (int c = c0; c <= c1; ++c)
            if (m[r][c] == SPACE) { 
                m[r][c] = depotType; 
                std::cout << "SETUP: depot type " << depotType << " placed at (" << r << "," << c << ") (fallback)" << std::endl;
                return; 
            }
}

// Setup terrain and depots
static void SetupMap() {
    std::cout << "=== SETUP: Initializing map " << MSZ << "x" << MSZ << " ===" << std::endl;
    for (int i = 0; i < MSZ; i++) {
        for (int j = 0; j < MSZ; j++) {
            map[i][j] = SPACE;
            smap[i][j] = 0.0;
        }
    }
    PlaceRandomCells(map, STONE, NUM_STONE);
    PlaceRandomCells(map, WATER, NUM_WATER);
    PlaceRandomCells(map, TREE, NUM_TREES);

    g_ammoDepots[TEAM_A].clear(); g_medDepots[TEAM_A].clear();
    g_ammoDepots[TEAM_B].clear(); g_medDepots[TEAM_B].clear();
    g_cellsTeamA.clear(); g_cellsTeamB.clear();

    int mid = MSZ / 2;

    // Team A depots
    PlaceDepotInRegion(map, DEPOT_AMMO, 0, MSZ - 1, 0, mid - 1);
    for (int r = 0; r < MSZ; ++r)
        for (int c = 0; c < mid; ++c)
            if (map[r][c] == DEPOT_AMMO) { RegisterDepotCell(TEAM_A, r, c, true); goto A_AMMO_DONE; }
A_AMMO_DONE:;
    PlaceDepotInRegion(map, DEPOT_MED, 0, MSZ - 1, 0, mid - 1);
    for (int r = 0; r < MSZ; ++r)
        for (int c = 0; c < mid; ++c)
            if (map[r][c] == DEPOT_MED) { RegisterDepotCell(TEAM_A, r, c, false); goto A_MED_DONE; }
A_MED_DONE:;

    // Team B depots
    PlaceDepotInRegion(map, DEPOT_AMMO, 0, MSZ - 1, mid, MSZ - 1);
    for (int r = 0; r < MSZ; ++r)
        for (int c = mid; c < MSZ; ++c)
            if (map[r][c] == DEPOT_AMMO) { RegisterDepotCell(TEAM_B, r, c, true); goto B_AMMO_DONE; }
B_AMMO_DONE:;
    PlaceDepotInRegion(map, DEPOT_MED, 0, MSZ - 1, mid, MSZ - 1);
    for (int r = 0; r < MSZ; ++r)
        for (int c = mid; c < MSZ; ++c)
            if (map[r][c] == DEPOT_MED) { RegisterDepotCell(TEAM_B, r, c, false); goto B_MED_DONE; }
B_MED_DONE:;

    assert(g_ammoDepots[TEAM_A].size() == 1);
    assert(g_medDepots[TEAM_A].size() == 1);
    assert(g_ammoDepots[TEAM_B].size() == 1);
    assert(g_medDepots[TEAM_B].size() == 1);
    
    std::cout << "SETUP: Team A depots - Ammo: (" << g_ammoDepots[TEAM_A][0].first << "," << g_ammoDepots[TEAM_A][0].second 
              << "), Med: (" << g_medDepots[TEAM_A][0].first << "," << g_medDepots[TEAM_A][0].second << ")" << std::endl;
    std::cout << "SETUP: Team B depots - Ammo: (" << g_ammoDepots[TEAM_B][0].first << "," << g_ammoDepots[TEAM_B][0].second 
              << "), Med: (" << g_medDepots[TEAM_B][0].first << "," << g_medDepots[TEAM_B][0].second << ")" << std::endl;
}

// Spawn teams
static void SetupTeams() {
    std::cout << "=== SETUP: Creating teams ===" << std::endl;
    g_teamA.CreateStandardSquad();
    g_teamB.CreateStandardSquad();
    int mid = MSZ / 2;
    bool okA = g_teamA.SpawnAllInRegion(map, 0, MSZ - 1, 0, mid - 1);
    bool okB = g_teamB.SpawnAllInRegion(map, 0, MSZ - 1, mid, MSZ - 1);
    assert(okA && "Failed to spawn Team A");
    assert(okB && "Failed to spawn Team B");
    std::cout << "SETUP: Teams spawned successfully" << std::endl;
}

// Attach environment
static void AttachEnvToTeams() {
    std::cout << "SETUP: Attaching environment maps to teams" << std::endl;
    g_teamA.AttachEnvironmentToAll(map, smap);
    g_teamB.AttachEnvironmentToAll(map, smap);
}

// Initialization
static void init() {
    srand(12345); // Deterministic seed for testing
    std::cout << "=== INITIALIZATION START ===" << std::endl;
    glClearColor(0.5, 0.7, 0.4, 0.0);
    glOrtho(0, MSZ, 0, MSZ, -1, 1);

    SetupMap();
    SetupTeams();
    AttachEnvToTeams();
    g_teamA.SetOwnDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
    g_teamA.SetKnownEnemyDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
    g_teamB.SetOwnDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
    g_teamB.SetKnownEnemyDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
    
    std::cout << "=== INITIALIZATION COMPLETE ===" << std::endl;
    std::cout << "Controls: T=Test Damage, A=Test Low Ammo, S=Toggle Security Map, R=Rebuild, Q=Quit" << std::endl;
}

// Draw entire map
static void ShowMap() {
    static int mapDrawCount = 0;
    if (++mapDrawCount % 500 == 0) { // Log every 500 map draws
        std::cout << "MAP: drawing map " << MSZ << "x" << MSZ << ", security map: " << (showSecurityMap ? "ON" : "OFF") << std::endl;
    }
    
    for (int i = 0; i < MSZ; i++) {
        for (int j = 0; j < MSZ; j++) {
            switch (map[i][j]) {
            case SPACE:
                if (showSecurityMap) {
                    double level = std::min(1.0, smap[i][j]);
                    glColor3d(1.0 - level, 1.0 - level, 1.0 - level);
                }
                else glColor3d(0.7, 0.9, 0.5);
                break;
            case STONE: glColor3d(0.3, 0.3, 0.3); break;
            case WATER: glColor3d(0.0, 1.0, 1.0); break;
            case TREE:  glColor3d(0.0, 0.6, 0.0); break;
            case DEPOT_AMMO:
                glColor3d(1.0, 1.0, 0.0);
                glBegin(GL_QUADS);
                glVertex2d(j, i); glVertex2d(j, i + 1); glVertex2d(j + 1, i + 1); glVertex2d(j + 1, i);
                glEnd();
                glColor3d(0, 0, 0);
                glBegin(GL_LINE_LOOP);
                glVertex2d(j + 0.05, i + 0.05);
                glVertex2d(j + 0.05, i + 0.95);
                glVertex2d(j + 0.95, i + 0.95);
                glVertex2d(j + 0.95, i + 0.05);
                glEnd();
                continue;
            case DEPOT_MED:
                glColor3d(1.0, 0.9, 0.2);
                glBegin(GL_QUADS);
                glVertex2d(j, i); glVertex2d(j, i + 1); glVertex2d(j + 1, i + 1); glVertex2d(j + 1, i);
                glEnd();
                glColor3d(0, 0, 0);
                glBegin(GL_LINE_LOOP);
                glVertex2d(j + 0.05, i + 0.05);
                glVertex2d(j + 0.05, i + 0.95);
                glVertex2d(j + 0.95, i + 0.95);
                glVertex2d(j + 0.95, i + 0.05);
                glEnd();
                continue;
            }
            glBegin(GL_QUADS);
            glVertex2d(j, i);
            glVertex2d(j, i + 1);
            glVertex2d(j + 1, i + 1);
            glVertex2d(j + 1, i);
            glEnd();
        }
    }
}

// Draw HUD with team status
static void DrawHUD() {
    static int hudDrawCount = 0;
    if (++hudDrawCount % 400 == 0) { // Log every 400 HUD draws
        std::cout << "HUD: drawing status display" << std::endl;
    }
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, W, 0, H, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Team A status (left side) - simplified without text rendering
    glColor3d(0.85, 0.15, 0.15);
    glRasterPos2d(10, H - 30);
    // Note: Text rendering removed due to GLUT compatibility issues
    
    auto warriorsA = g_teamA.GetWarriors();
    int y = H - 50;
    for (size_t i = 0; i < warriorsA.size(); ++i) {
        if (warriorsA[i]->IsAlive()) {
            // Draw colored rectangles instead of text for HP/Ammo status
            glColor3d(0.9, 0.1, 0.1);
            glBegin(GL_QUADS);
            glVertex2d(20, y);
            glVertex2d(20 + warriorsA[i]->HP() / 10.0, y);
            glVertex2d(20 + warriorsA[i]->HP() / 10.0, y + 10);
            glVertex2d(20, y + 10);
            glEnd();
            
            glColor3d(0.1, 0.9, 0.1);
            glBegin(GL_QUADS);
            glVertex2d(20, y + 12);
            glVertex2d(20 + warriorsA[i]->Ammo() / 2.0, y + 12);
            glVertex2d(20 + warriorsA[i]->Ammo() / 2.0, y + 22);
            glVertex2d(20, y + 22);
            glEnd();
            
            y -= 30;
        }
    }
    
    // Team B status (right side) - simplified without text rendering
    glColor3d(0.15, 0.35, 0.95);
    glRasterPos2d(W - 100, H - 30);
    // Note: Text rendering removed due to GLUT compatibility issues
    
    auto warriorsB = g_teamB.GetWarriors();
    y = H - 50;
    for (size_t i = 0; i < warriorsB.size(); ++i) {
        if (warriorsB[i]->IsAlive()) {
            // Draw colored rectangles instead of text for HP/Ammo status
            glColor3d(0.1, 0.1, 0.9);
            glBegin(GL_QUADS);
            glVertex2d(W - 90, y);
            glVertex2d(W - 90 + warriorsB[i]->HP() / 10.0, y);
            glVertex2d(W - 90 + warriorsB[i]->HP() / 10.0, y + 10);
            glVertex2d(W - 90, y + 10);
            glEnd();
            
            glColor3d(0.1, 0.9, 0.1);
            glBegin(GL_QUADS);
            glVertex2d(W - 90, y + 12);
            glVertex2d(W - 90 + warriorsB[i]->Ammo() / 2.0, y + 12);
            glVertex2d(W - 90 + warriorsB[i]->Ammo() / 2.0, y + 22);
            glVertex2d(W - 90, y + 22);
            glEnd();
            
            y -= 30;
        }
    }
    
    // Commander mode indicator - simplified
    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2d(W/2 - 50, H - 30);
    glVertex2d(W/2 + 50, H - 30);
    glVertex2d(W/2 + 50, H - 20);
    glVertex2d(W/2 - 50, H - 20);
    glEnd();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Display
static void display() {
    static int displayCount = 0;
    if (++displayCount % 600 == 0) { // Log every 600 displays
        std::cout << "DISPLAY: rendering frame " << displayCount << std::endl;
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    ShowMap();
    if (pg) pg->Show();
    g_teamA.Draw();
    g_teamB.Draw();
    DrawHUD();
    glutSwapBuffers();
}

// Idle: update map and teams
static void idle() {
    static int frameCount = 0;
    frameCount++;
    
    if (pg) {
        pg->Explode(map);
        if (frameCount % 60 == 0) { // Log every 60 frames
            std::cout << "IDLE: Frame " << frameCount << " - updating teams and map" << std::endl;
        }
    }
    g_teamA.Update(map, smap);
    g_teamB.Update(map, smap);
    glutPostRedisplay();
}

// Mouse: throw grenade
static void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        double xx = MSZ * x / static_cast<double>(W);
        double yy = MSZ * (H - y - 1) / static_cast<double>(H);
        std::cout << "MOUSE: Left click at (" << xx << "," << yy << ") - creating grenade" << std::endl;
        pg = new Grenade(xx, yy);
        pg->Explode(map);
    }
}

// Keyboard: debug support
static void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 't': case 'T': {
        std::cout << "KEY: T pressed - testing damage on Team A Warrior 1" << std::endl;
        auto warriors = g_teamA.GetWarriors();
        if (!warriors.empty() && warriors[0]->IsAlive()) warriors[0]->TestDamage();
    } break;
    case 'a': case 'A': {
        std::cout << "KEY: A pressed - testing low ammo on Team A Warrior 1" << std::endl;
        auto warriors = g_teamA.GetWarriors();
        if (!warriors.empty() && warriors[0]->IsAlive()) warriors[0]->TestLowAmmo();
    } break;
    case 's': case 'S':
        showSecurityMap = !showSecurityMap;
        std::cout << "KEY: S pressed - Security map: " << (showSecurityMap ? "ON" : "OFF") << std::endl;
        break;
    case 'r': case 'R':
        std::cout << "KEY: R pressed - rebuilding map and teams" << std::endl;
        SetupMap(); SetupTeams(); AttachEnvToTeams();
        g_teamA.SetOwnDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
        g_teamA.SetKnownEnemyDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetOwnDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetKnownEnemyDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
        break;
    case 'q': case 'Q': case 27: 
        std::cout << "KEY: Q/ESC pressed - exiting" << std::endl;
        exit(0); 
        break;
    }
}

// Right-click menu
static void menu(int option) {
    if (option == 1) {
        showSecurityMap = true;
        std::cout << "MENU: Show Security Map selected" << std::endl;
    }
    else if (option == 2) {
        showSecurityMap = false;
        std::cout << "MENU: Hide Security Map selected" << std::endl;
    }
    else if (option == 3) {
        std::cout << "MENU: Rebuild Map + Teams selected" << std::endl;
        SetupMap(); SetupTeams(); AttachEnvToTeams();
        g_teamA.SetOwnDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
        g_teamA.SetKnownEnemyDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetOwnDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetKnownEnemyDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== MAIN: Starting AI Battle Simulation ===" << std::endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("AI Battle Simulation - Teams & Security Map");

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboard);

    glutCreateMenu(menu);
    glutAddMenuEntry("Show Security Map", 1);
    glutAddMenuEntry("Hide Security Map", 2);
    glutAddMenuEntry("Rebuild Map + Teams", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    std::cout << "=== MAIN: Initializing OpenGL and game systems ===" << std::endl;
    init();
    std::cout << "=== MAIN: Entering main loop ===" << std::endl;
    glutMainLoop();
    return 0;
}
