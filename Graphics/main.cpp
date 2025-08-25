// main.cpp
// Renders terrain, depots, optional security map, and two OOP-based teams.
// Team A is red (left half), Team B is blue (right half).

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>
#include <unordered_set>
#include <cassert>

#include "glut.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Definitions.h"
#include "Agent.h"
#include "Team.h"

using std::pair;
using std::vector;
using std::unordered_set;

// ----------------------------
// Content amounts per terrain
// ----------------------------
const int NUM_STONE = 200;
const int NUM_WATER = 50;
const int NUM_TREES = 100;

// ----------------------------
// Window size in pixels
// ----------------------------
const int W = 700;
const int H = 700;

// ----------------------------
// Global maps & flags
// ----------------------------
int    map[MSZ][MSZ] = { 0 };  // terrain map
double smap[MSZ][MSZ] = { 0 };  // security/danger visualization (0..1)
bool   showSecurityMap = false;

// ----------------------------
// Teams (OOP)
// ----------------------------
TeamSquad g_teamA(TEAM_A);
TeamSquad g_teamB(TEAM_B);

// ----------------------------
// Exact cells of each team's depots (ammo/med)
// ----------------------------
vector<pair<int, int>> g_ammoDepots[2];
vector<pair<int, int>> g_medDepots[2];

// Quick membership sets for drawing (encode (r,c) as r*MSZ + c)
inline int CellKey(int r, int c) { return r * MSZ + c; }
unordered_set<int> g_cellsTeamA;
unordered_set<int> g_cellsTeamB;

// ----------------------------
// Visual grenade for risk map demo
// ----------------------------
Bullet* pb = nullptr;
Grenade* pg = nullptr;

// -------------------------------------------------------
// Utility: record the team ownership of a depot cell
// -------------------------------------------------------
static void RegisterDepotCell(Team team, int r, int c, bool isAmmo)
{
    if (team == TEAM_A) g_cellsTeamA.insert(CellKey(r, c));
    else                g_cellsTeamB.insert(CellKey(r, c));

    if (isAmmo) g_ammoDepots[team].push_back({ r, c });
    else        g_medDepots[team].push_back({ r, c });
}

// -------------------------------------------------------
// Scatter 'count' cells of value 'cellValue' on SPACE
// -------------------------------------------------------
static void PlaceRandomCells(int m[MSZ][MSZ], int cellValue, int count)
{
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

// -------------------------------------------------------
// Place a single depot randomly inside [r0..r1]x[c0..c1]
// Keeps a small margin so the border looks nicer
// -------------------------------------------------------
static void PlaceDepotInRegion(int m[MSZ][MSZ],
    int depotType,
    int r0, int r1, int c0, int c1)
{
    const int margin = 2;
    r0 = std::max(r0 + margin, 0);
    c0 = std::max(c0 + margin, 0);
    r1 = std::min(r1 - margin, MSZ - 1);
    c1 = std::min(c1 - margin, MSZ - 1);

    // Try random positions first
    for (int tries = 0; tries < 1000; ++tries) {
        int r = r0 + rand() % (r1 - r0 + 1);
        int c = c0 + rand() % (c1 - c0 + 1);
        if (m[r][c] == SPACE) {
            m[r][c] = depotType;
            return;
        }
    }
    // Fallback: deterministic scan
    for (int r = r0; r <= r1; ++r)
        for (int c = c0; c <= c1; ++c)
            if (m[r][c] == SPACE) { m[r][c] = depotType; return; }
}

// -------------------------------------------------------
// Manhattan distance on grid (for future AI usage)
// -------------------------------------------------------
inline int Mdist(int r1, int c1, int r2, int c2) {
    return std::abs(r1 - r2) + std::abs(c1 - c2);
}

// -------------------------------------------------------
// Nearest depot queries (for future AI usage)
// -------------------------------------------------------
std::pair<int, int> FindNearestAmmoDepot(Team team, int r, int c) {
    std::pair<int, int> best = { -1, -1 };
    int bestD = 1e9;
    for (auto& p : g_ammoDepots[team]) {
        int d = Mdist(r, c, p.first, p.second);
        if (d < bestD) { bestD = d; best = p; }
    }
    return best;
}

std::pair<int, int> FindNearestMedDepot(Team team, int r, int c) {
    std::pair<int, int> best = { -1, -1 };
    int bestD = 1e9;
    for (auto& p : g_medDepots[team]) {
        int d = Mdist(r, c, p.first, p.second);
        if (d < bestD) { bestD = d; best = p; }
    }
    return best;
}

// -------------------------------------------------------
// Map setup: terrain + 2 depots (ammo+med), one per team half
// Left half is Team A, right half is Team B
// -------------------------------------------------------
static void SetupMap()
{
    // Reset terrain and security maps
    for (int i = 0; i < MSZ; i++)
        for (int j = 0; j < MSZ; j++) {
            map[i][j] = SPACE;
            smap[i][j] = 0.0;
        }

    // Scatter simple single-cell terrain
    PlaceRandomCells(map, STONE, NUM_STONE);
    PlaceRandomCells(map, WATER, NUM_WATER);
    PlaceRandomCells(map, TREE, NUM_TREES);

    // Clear previous depot ownership data
    g_ammoDepots[TEAM_A].clear(); g_ammoDepots[TEAM_B].clear();
    g_medDepots[TEAM_A].clear();  g_medDepots[TEAM_B].clear();
    g_cellsTeamA.clear();         g_cellsTeamB.clear();

    // Split map by columns
    int mid = MSZ / 2;

    // --- Team A (left half: cols 0..mid-1)
    {
        // Place AMMO
        PlaceDepotInRegion(map, DEPOT_AMMO, 0, MSZ - 1, 0, mid - 1);
        for (int r = 0; r < MSZ; ++r)
            for (int c = 0; c < mid; ++c)
                if (map[r][c] == DEPOT_AMMO) { RegisterDepotCell(TEAM_A, r, c, /*isAmmo=*/true); goto A_AMMO_DONE; }
    A_AMMO_DONE:;

        // Place MED
        PlaceDepotInRegion(map, DEPOT_MED, 0, MSZ - 1, 0, mid - 1);
        for (int r = 0; r < MSZ; ++r)
            for (int c = 0; c < mid; ++c)
                if (map[r][c] == DEPOT_MED) { RegisterDepotCell(TEAM_A, r, c, /*isAmmo=*/false); goto A_MED_DONE; }
    A_MED_DONE:;
    }

    // --- Team B (right half: cols mid..MSZ-1)
    {
        // Place AMMO
        PlaceDepotInRegion(map, DEPOT_AMMO, 0, MSZ - 1, mid, MSZ - 1);
        for (int r = 0; r < MSZ; ++r)
            for (int c = mid; c < MSZ; ++c)
                if (map[r][c] == DEPOT_AMMO) { RegisterDepotCell(TEAM_B, r, c, /*isAmmo=*/true); goto B_AMMO_DONE; }
    B_AMMO_DONE:;

        // Place MED
        PlaceDepotInRegion(map, DEPOT_MED, 0, MSZ - 1, mid, MSZ - 1);
        for (int r = 0; r < MSZ; ++r)
            for (int c = mid; c < MSZ; ++c)
                if (map[r][c] == DEPOT_MED) { RegisterDepotCell(TEAM_B, r, c, /*isAmmo=*/false); goto B_MED_DONE; }
    B_MED_DONE:;
    }

    // Optional sanity checks in debug builds
    assert(g_ammoDepots[TEAM_A].size() == 1);
    assert(g_medDepots[TEAM_A].size() == 1);
    assert(g_ammoDepots[TEAM_B].size() == 1);
    assert(g_medDepots[TEAM_B].size() == 1);
}

// -------------------------------------------------------
// Build both teams and spawn them on their map halves
// -------------------------------------------------------
static void SetupTeams()
{
    g_teamA.CreateStandardSquad();
    g_teamB.CreateStandardSquad();

    const int mid = MSZ / 2;

    // Left half: Team A
    bool okA = g_teamA.SpawnAllInRegion(map, 0, MSZ - 1, 0, mid - 1);
    // Right half: Team B
    bool okB = g_teamB.SpawnAllInRegion(map, 0, MSZ - 1, mid, MSZ - 1);

    // If spawn fails (map too dense), you can rebuild or assert
    assert(okA && "Failed to spawn Team A");
    assert(okB && "Failed to spawn Team B");
}

// -------------------------------------------------------
// OpenGL init: world coords are [0..MSZ]x[0..MSZ]
// so each map cell is exactly 1x1 in world space
// -------------------------------------------------------
static void init()
{
    srand(static_cast<unsigned>(time(nullptr)));
    glClearColor(0.5, 0.7, 0.4, 0.0);
    glOrtho(0, MSZ, 0, MSZ, -1, 1);

    SetupMap();
    SetupTeams();
}

// -------------------------------------------------------
// Draw the current map state
// Depots are filled with yellow-ish and a thin black stroke
// SPACE tiles optionally show security shading
// -------------------------------------------------------
static void ShowMap()
{
    for (int i = 0; i < MSZ; ++i)
        for (int j = 0; j < MSZ; ++j)
        {
            switch (map[i][j])
            {
            case SPACE:
                if (showSecurityMap) {
                    double level = smap[i][j];
                    glColor3d(1.0 - level, 1.0 - level, 1.0 - level); // darker = less safe
                }
                else {
                    glColor3d(0.7, 0.9, 0.5);
                }
                break;

            case STONE: glColor3d(0.3, 0.3, 0.3); break;
            case WATER: glColor3d(0.0, 1.0, 1.0); break; // cyan
            case TREE:  glColor3d(0.0, 0.6, 0.0); break; // dark green

            case DEPOT_AMMO:
            {
                // fill
                glColor3d(1.0, 1.0, 0.0);
                glBegin(GL_QUADS);
                glVertex2d(j, i);
                glVertex2d(j, i + 1);
                glVertex2d(j + 1, i + 1);
                glVertex2d(j + 1, i);
                glEnd();

                // stroke
                const double eps = 0.05;
                glLineWidth(1.0f);
                glColor3d(0.0, 0.0, 0.0);
                glBegin(GL_LINE_LOOP);
                glVertex2d(j + eps, i + eps);
                glVertex2d(j + eps, i + 1 - eps);
                glVertex2d(j + 1 - eps, i + 1 - eps);
                glVertex2d(j + 1 - eps, i + eps);
                glEnd();
                glLineWidth(1.0f);
                continue; // skip generic quad below
            }

            case DEPOT_MED:
            {
                // fill
                glColor3d(1.0, 0.9, 0.2);
                glBegin(GL_QUADS);
                glVertex2d(j, i);
                glVertex2d(j, i + 1);
                glVertex2d(j + 1, i + 1);
                glVertex2d(j + 1, i);
                glEnd();

                // stroke
                const double eps = 0.05;
                glLineWidth(1.0f);
                glColor3d(0.0, 0.0, 0.0);
                glBegin(GL_LINE_LOOP);
                glVertex2d(j + eps, i + eps);
                glVertex2d(j + eps, i + 1 - eps);
                glVertex2d(j + 1 - eps, i + 1 - eps);
                glVertex2d(j + 1 - eps, i + eps);
                glEnd();
                glLineWidth(1.0f);
                continue; // skip generic quad below
            }
            } // end switch

            // generic 1x1 cell quad
            glBegin(GL_QUADS);
            glVertex2d(j, i);
            glVertex2d(j, i + 1);
            glVertex2d(j + 1, i + 1);
            glVertex2d(j + 1, i);
            glEnd();
        }
}

// -------------------------------------------------------
// Build a security map using multiple virtual grenades
// (kept from your base for visualization/testing)
// -------------------------------------------------------
static void CreateSecurityMap()
{
    const int numGrenades = 10;
    for (int k = 0; k < numGrenades; ++k) {
        int x = MSZ / 2 + rand() % 20 - 10;
        int y = MSZ / 2 + rand() % 20 - 10;
        pg = new Grenade(x, y);
        pg->CreateScurityMap(map, smap);
    }
}

// -------------------------------------------------------
// GLUT display / idle / mouse / menu
// -------------------------------------------------------
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Terrain and depots
    ShowMap();

    // Optional security grenades visualization
    if (pg != nullptr)
        pg->Show();

    // Draw teams (5 agents each: C, W, W, M, P)
    g_teamA.Draw();
    g_teamB.Draw();

    glutSwapBuffers();
}

static void idle()
{
    if (pg != nullptr)
        pg->Explode(map);

    glutPostRedisplay();
}

static void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Map window-pixels to world-grid coordinates [0..MSZ]
        double xx = MSZ * x / static_cast<double>(W);
        double yy = MSZ * (H - y - 1) / static_cast<double>(H);

        pg = new Grenade(xx, yy);
        pg->Explode(map);
    }
}

static void menu(int option)
{
    if (option == 1) {
        showSecurityMap = true;
        CreateSecurityMap();
    }
    else if (option == 2) {
        showSecurityMap = false;
    }
    else if (option == 3) {
        // Rebuild terrain and teams
        SetupMap();
        SetupTeams();
    }
}

// -------------------------------------------------------
// Program entry
// -------------------------------------------------------
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(400, 100);
    glutCreateWindow("Teams & Security Map");

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMouseFunc(mouseClick);

    glutCreateMenu(menu);
    glutAddMenuEntry("Show Security Map", 1);
    glutAddMenuEntry("Hide Security Map", 2);
    glutAddMenuEntry("Rebuild Map + Teams", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    init();
    glutMainLoop();
    return 0;
}
