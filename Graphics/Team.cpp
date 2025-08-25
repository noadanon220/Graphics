#include "Team.h"
#include <algorithm>
#include <cstdlib>

TeamSquad::TeamSquad(Team id) : teamId(id) {}

void TeamSquad::CreateStandardSquad() {
    members.clear();
    // 1 Commander
    members.emplace_back(std::make_unique<Agent>(teamId, CMD));
    // 2 Warriors
    members.emplace_back(std::make_unique<Agent>(teamId, WAR));
    members.emplace_back(std::make_unique<Agent>(teamId, WAR));
    // 1 Medic
    members.emplace_back(std::make_unique<Agent>(teamId, MED));
    // 1 Supplier
    members.emplace_back(std::make_unique<Agent>(teamId, SUP));
}

bool TeamSquad::IsFreeForAgent(int map[MSZ][MSZ], int r, int c) {
    if (r < 0 || r >= MSZ || c < 0 || c >= MSZ) return false;
    return map[r][c] == SPACE;
}

bool TeamSquad::RandomFreeCellInRegion(int map[MSZ][MSZ],
    int r0, int r1, int c0, int c1,
    int& outR, int& outC, int maxTries)
{
    r0 = std::max(0, r0); c0 = std::max(0, c0);
    r1 = std::min(MSZ - 1, r1); c1 = std::min(MSZ - 1, c1);

    for (int t = 0; t < maxTries; ++t) {
        int r = r0 + rand() % (r1 - r0 + 1);
        int c = c0 + rand() % (c1 - c0 + 1);
        if (IsFreeForAgent(map, r, c)) { outR = r; outC = c; return true; }
    }
    // Fallback deterministic scan
    for (int r = r0; r <= r1; ++r)
        for (int c = c0; c <= c1; ++c)
            if (IsFreeForAgent(map, r, c)) { outR = r; outC = c; return true; }
    return false;
}

bool TeamSquad::SpawnAllInRegion(int map[MSZ][MSZ], int r0, int r1, int c0, int c1)
{
    const int margin = 3;
    r0 += margin; c0 += margin; r1 -= margin; c1 -= margin;

    for (auto& m : members) {
        int rr, cc;
        if (!RandomFreeCellInRegion(map, r0, r1, c0, c1, rr, cc))
            return false;
        m->SetPosition(rr, cc);
    }
    return true;
}

void TeamSquad::Draw() const {
    for (const auto& m : members)
        m->Draw();
}
