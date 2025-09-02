#include "Visibility.h"
#include <cmath>
#include <algorithm>
#include <iostream> // for logging

// Simple line-of-sight using Bresenham's algorithm; stops at blocking cells.
static bool LineOfSight(int map[MSZ][MSZ], int r0, int c0, int r1, int c1) {
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    int sr = (r0 < r1) ? 1 : -1;
    int sc = (c0 < c1) ? 1 : -1;
    int err = dc - dr;
    int r = r0, c = c0;
    while (!(r == r1 && c == c1)) {
        int e2 = 2 * err;
        if (e2 > -dr) { err -= dr; c += sc; }
        if (e2 < dc) { err += dc; r += sr; }
        if (r == r1 && c == c1) break;
        if (r < 0 || r >= MSZ || c < 0 || c >= MSZ) return false;
        if (BlocksVision(map[r][c])) {
            static int visionBlockedCount = 0;
            if (++visionBlockedCount % 600 == 0) { // Log every 600 blocks
                std::cout << "LineOfSight: vision blocked at (" << r << "," << c << ") by terrain type " << map[r][c] << std::endl;
            }
            return false;
        }
    }
    return true;
}

void BuildVisibilityForUnit(int map[MSZ][MSZ], int r0, int c0, double outV[MSZ][MSZ], int maxRange) {
    for (int r = 0; r < MSZ; ++r)
        for (int c = 0; c < MSZ; ++c)
            outV[r][c] = 0.0;
    int rmin = std::max(0, r0 - maxRange), rmax = std::min(MSZ - 1, r0 + maxRange);
    int cmin = std::max(0, c0 - maxRange), cmax = std::min(MSZ - 1, c0 + maxRange);
    int visibleCells = 0;
    for (int r = rmin; r <= rmax; ++r) {
        for (int c = cmin; c <= cmax; ++c) {
            int dist = std::abs(r - r0) + std::abs(c - c0);
            if (dist > maxRange) continue;
            if (!LineOfSight(map, r0, c0, r, c)) continue;
            double v = 1.0 - (double)dist / (double)std::max(1, maxRange);
            outV[r][c] = std::max(outV[r][c], v);
            visibleCells++;
        }
    }
    static int callCount = 0;
    if (++callCount % 100 == 0) { // Log every 100 calls
        std::cout << "Visibility: unit at (" << r0 << "," << c0 << ") sees " << visibleCells 
                  << " cells in range " << maxRange << std::endl;
    }
}
