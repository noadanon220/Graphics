#pragma once
#include <vector>
#include <utility>
#include "Definitions.h"

// A* minimizing risk: cost = 1 + riskWeight*smap[r][c]
std::vector<std::pair<int, int>> RiskAwareAStar(
    int map[MSZ][MSZ],
    double smap[MSZ][MSZ],
    int sr, int sc,
    int gr, int gc,
    double riskWeight);

// A* combining risk and visibility: used by the commander
// cost = 1 + riskW*smap + visW*(1 - clamp(vis[r][c],0,1))
std::vector<std::pair<int, int>> RiskVisAStar(
    int map[MSZ][MSZ],
    double smap[MSZ][MSZ],
    double vis[MSZ][MSZ],
    int sr, int sc,
    int gr, int gc,
    double riskWeight,
    double visWeight);
