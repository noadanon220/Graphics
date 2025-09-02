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
    if (team == TEAM_A) g_cellsTeamA.insert(CellKey(r, c));
    else g_cellsTeamB.insert(CellKey(r, c));
    if (isAmmo) g_ammoDepots[team].push_back({ r,c });
    else g_medDepots[team].push_back({ r,c });
}

// Random scatter function
static void PlaceRandomCells(int m[MSZ][MSZ], int cellValue, int count) {
    int placed = 0;
    while (placed < count) {
        int r = rand() % MSZ;
        int c = rand() % MSZ;
        if (m[r][c] == SPACE) {
            m[r][c] = cellValue;
            placed++;
        }
    }
}

// Place depot in subregion
static void PlaceDepotInRegion(int m[MSZ][MSZ], int depotType,
    int r0, int r1, int c0, int c1)
{
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
            return;
        }
    }
    for (int r = r0; r <= r1; ++r)
        for (int c = c0; c <= c1; ++c)
            if (m[r][c] == SPACE) {
                m[r][c] = depotType;
                return;
            }
}

// Setup terrain and depots
static void SetupMap() {
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
}

// Spawn teams
static void SetupTeams() {
    g_teamA.CreateStandardSquad();
    g_teamB.CreateStandardSquad();
    int mid = MSZ / 2;
    bool okA = g_teamA.SpawnAllInRegion(map, 0, MSZ - 1, 0, mid - 1);
    bool okB = g_teamB.SpawnAllInRegion(map, 0, MSZ - 1, mid, MSZ - 1);
    assert(okA && "Failed to spawn Team A");
    assert(okB && "Failed to spawn Team B");
}

// Attach environment
static void AttachEnvToTeams() {
    g_teamA.AttachEnvironmentToAll(map, smap);
    g_teamB.AttachEnvironmentToAll(map, smap);
}

// Initialization
static void init() {
    srand(12345); // Deterministic seed for testing
    glClearColor(0.5, 0.7, 0.4, 0.0);
    glOrtho(0, MSZ, 0, MSZ, -1, 1);

    SetupMap();
    SetupTeams();
    AttachEnvToTeams();

    // NEW: wire opponents
    g_teamA.SetOpponent(&g_teamB);
    g_teamB.SetOpponent(&g_teamA);

    g_teamA.SetOwnDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
    g_teamA.SetKnownEnemyDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
    g_teamB.SetOwnDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
    g_teamB.SetKnownEnemyDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
}

// Draw entire map
static void ShowMap() {
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
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, W, 0, H, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Team A status (left side) - rectangles for HP/Ammo
    auto warriorsA = g_teamA.GetWarriors();
    int y = H - 50;
    for (size_t i = 0; i < warriorsA.size(); ++i) {
        if (warriorsA[i]->IsAlive()) {
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

    // Team B status (right side)
    auto warriorsB = g_teamB.GetWarriors();
    y = H - 50;
    for (size_t i = 0; i < warriorsB.size(); ++i) {
        if (warriorsB[i]->IsAlive()) {
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

    // Commander mode indicator (simple bar)
    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2d(W / 2 - 50, H - 30);
    glVertex2d(W / 2 + 50, H - 30);
    glVertex2d(W / 2 + 50, H - 20);
    glVertex2d(W / 2 - 50, H - 20);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Display
static void display() {
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
        pg->CreateScurityMap(map, smap);
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
        pg = new Grenade(xx, yy);
        pg->Explode(map);
    }
}

// Keyboard: debug support
static void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 't': case 'T': {
        auto warriors = g_teamA.GetWarriors();
        if (!warriors.empty() && warriors[0]->IsAlive()) warriors[0]->TestDamage();
    } break;
    case 'a': case 'A': {
        auto warriors = g_teamA.GetWarriors();
        if (!warriors.empty() && warriors[0]->IsAlive()) warriors[0]->TestLowAmmo();
    } break;
    case 's': case 'S':
        showSecurityMap = !showSecurityMap;
        break;
    case 'r': case 'R':
        SetupMap(); SetupTeams(); AttachEnvToTeams();
        // NEW: re-wire opponents after rebuild
        g_teamA.SetOpponent(&g_teamB);
        g_teamB.SetOpponent(&g_teamA);

        g_teamA.SetOwnDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
        g_teamA.SetKnownEnemyDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetOwnDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetKnownEnemyDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
        break;
    case 'q': case 'Q': case 27:
        exit(0);
        break;
    }
}

// Right-click menu
static void menu(int option) {
    if (option == 1) {
        showSecurityMap = true;
    }
    else if (option == 2) {
        showSecurityMap = false;
    }
    else if (option == 3) {
        SetupMap(); SetupTeams(); AttachEnvToTeams();

        // NEW: re-wire opponents after rebuild
        g_teamA.SetOpponent(&g_teamB);
        g_teamB.SetOpponent(&g_teamA);

        g_teamA.SetOwnDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
        g_teamA.SetKnownEnemyDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetOwnDepots(g_ammoDepots[TEAM_B], g_medDepots[TEAM_B]);
        g_teamB.SetKnownEnemyDepots(g_ammoDepots[TEAM_A], g_medDepots[TEAM_A]);
    }
}

int main(int argc, char* argv[]) {
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

    init();
    glutMainLoop();
    return 0;
}
