#include "AStar.h"
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <iostream> // for logging

static inline bool InBounds(int r, int c) { return r >= 0 && r < MSZ && c >= 0 && c < MSZ; }
static inline int Key(int r, int c) { return r * MSZ + c; }
static inline double H(int r1, int c1, int r2, int c2) { return std::abs(r1 - r2) + std::abs(c1 - c2); }

struct Rec { int r, c; double g, f; };
struct Cmp { bool operator()(const Rec& a, const Rec& b)const { return a.f > b.f; } };

std::vector<std::pair<int, int>> RiskAwareAStar(
    int map[MSZ][MSZ], double smap[MSZ][MSZ],
    int sr, int sc, int gr, int gc,
    double riskWeight)
{
    std::vector<std::pair<int, int>> path;
    if (!InBounds(sr, sc) || !InBounds(gr, gc)) return path;
    if (BlocksMovement(map[sr][sc]) || BlocksMovement(map[gr][gc])) return path;

    std::priority_queue<Rec, std::vector<Rec>, Cmp> open;
    std::unordered_map<int, double> gscore;
    std::unordered_map<int, int> came;

    auto push = [&](int r, int c, double g, int par) {
        int k = Key(r, c);
        if (!gscore.count(k) || g < gscore[k]) {
            gscore[k] = g;
            came[k] = par;
            open.push({ r,c,g, g + H(r,c,gr,gc) });
        }
        };

    push(sr, sc, 0.0, -1);

    const int dr[4] = { -1,1,0,0 };
    const int dc[4] = { 0,0,-1,1 };

    while (!open.empty()) {
        Rec cur = open.top(); open.pop();

        if (cur.r == gr && cur.c == gc) {
            int k = Key(cur.r, cur.c);
            while (k != -1) {
                int r = k / MSZ;
                int c = k % MSZ;
                path.emplace_back(r, c);
                auto it = came.find(k);
                if (it == came.end()) break;
                k = it->second;
            }
            std::reverse(path.begin(), path.end());
            if (path.size() > 1) {
                std::cout << "A* Risk: path (" << sr << "," << sc << ") -> (" << gr << "," << gc 
                          << ") length " << path.size() << ", cost " << cur.g << std::endl;
            }
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int nr = cur.r + dr[i];
            int nc = cur.c + dc[i];
            if (!InBounds(nr, nc) || BlocksMovement(map[nr][nc])) continue;
            double step = 1.0 + riskWeight * smap[nr][nc];
            double ng = cur.g + step;
            push(nr, nc, ng, Key(cur.r, cur.c));
        }
    }
    std::cout << "A* Risk: no path found from (" << sr << "," << sc << ") to (" << gr << "," << gc << ")" << std::endl;
    return path;
}

std::vector<std::pair<int, int>> RiskVisAStar(
    int map[MSZ][MSZ], double smap[MSZ][MSZ], double vis[MSZ][MSZ],
    int sr, int sc, int gr, int gc,
    double riskW, double visW)
{
    std::vector<std::pair<int, int>> path;
    if (!InBounds(sr, sc) || !InBounds(gr, gc)) return path;
    if (BlocksMovement(map[sr][sc]) || BlocksMovement(map[gr][gc])) return path;

    std::priority_queue<Rec, std::vector<Rec>, Cmp> open;
    std::unordered_map<int, double> gscore;
    std::unordered_map<int, int> came;

    auto push = [&](int r, int c, double g, int par) {
        int k = Key(r, c);
        if (!gscore.count(k) || g < gscore[k]) {
            gscore[k] = g;
            came[k] = par;
            open.push({ r,c,g, g + H(r,c,gr,gc) });
        }
        };

    push(sr, sc, 0.0, -1);

    const int dr[4] = { -1,1,0,0 };
    const int dc[4] = { 0,0,-1,1 };

    while (!open.empty()) {
        Rec cur = open.top(); open.pop();
        if (cur.r == gr && cur.c == gc) {
            int k = Key(cur.r, cur.c);
            while (k != -1) {
                int r = k / MSZ; int c = k % MSZ;
                path.emplace_back(r, c);
                auto it = came.find(k);
                if (it == came.end()) break;
                k = it->second;
            }
            std::reverse(path.begin(), path.end());
            if (path.size() > 1) {
                std::cout << "A* RiskVis: path (" << sr << "," << sc << ") -> (" << gr << "," << gc 
                          << ") length " << path.size() << ", cost " << cur.g << std::endl;
            }
            return path;
        }
        for (int i = 0; i < 4; ++i) {
            int nr = cur.r + dr[i], nc = cur.c + dc[i];
            if (!InBounds(nr, nc) || BlocksMovement(map[nr][nc])) continue;
            double v = std::min(1.0, std::max(0.0, vis[nr][nc]));
            double step = 1.0 + riskW * smap[nr][nc] + visW * (1.0 - v);
            double ng = cur.g + step;
            push(nr, nc, ng, Key(cur.r, cur.c));
        }
    }
    std::cout << "A* RiskVis: no path found from (" << sr << "," << sc << ") to (" << gr << "," << gc << ")" << std::endl;
    return path;
}
