// WarriorStates.cpp
#include "WarriorStates.h"
#include "WarriorNPC.h"
#include "AStar.h"
#include "Definitions.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

// Local line-of-fire for picking legal firing positions
static bool LineOfFireWS(int map[MSZ][MSZ], int r0, int c0, int r1, int c1) {
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    int sr = (r0 < r1) ? 1 : -1, sc = (c0 < c1) ? 1 : -1;
    int err = dc - dr, r = r0, c = c0;
    while (!(r == r1 && c == c1)) {
        int e2 = 2 * err;
        if (e2 > -dr) { err -= dr; c += sc; }
        if (e2 < dc) { err += dc; r += sr; }
        if (r == r1 && c == c1) break;
        if (r < 0 || r >= MSZ || c < 0 || c >= MSZ) return false;
        if (BlocksFire(map[r][c])) return false;
    }
    return true;
}

// ===== WarriorIdle =====
void WarriorIdle::OnEnter(NPC* np) { (void)np; }
void WarriorIdle::Transition(NPC* /*np*/) { /* stay idle */ }
void WarriorIdle::OnExit(NPC* /*np*/) {}

// ===== WarriorMoveTo =====
void WarriorMoveTo::OnEnter(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    auto tgt = w->GetMissionTarget();
    if (tgt.first == -1 || !w->EnvMap() || !w->EnvSMap()) { w->setIsMoving(false); return; }

    auto path = RiskAwareAStar(w->EnvMap(), w->EnvSMap(),
        w->Row(), w->Col(), tgt.first, tgt.second,
        w->GetRiskWeight());
    if (!path.empty()) { w->SetPathCells(path); w->setIsMoving(true); }
    else { w->setIsMoving(false); }
}
void WarriorMoveTo::Transition(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    auto tgt = w->GetMissionTarget();
    int dist = std::abs(w->Row() - tgt.first) + std::abs(w->Col() - tgt.second);
    if (dist <= 1) {
        OnExit(np);
        w->setCurrentState(new WarriorIdle());
        w->getCurrentState()->OnEnter(w);
    }
}
void WarriorMoveTo::OnExit(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    w->setIsMoving(false);
}

// ===== WarriorAttack =====
void WarriorAttack::OnEnter(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    auto tgt = w->GetMissionTarget();
    if (tgt.first == -1 || !w->EnvMap() || !w->EnvSMap()) { w->setIsMoving(false); return; }

    int (*map)[MSZ] = w->EnvMap();
    double (*smap)[MSZ] = w->EnvSMap();

    int bestR = -1, bestC = -1;
    double bestCost = 1e18;

    // Search around target for a legal firing position
    for (int rr = tgt.first - 12; rr <= tgt.first + 12; ++rr) {
        for (int cc = tgt.second - 12; cc <= tgt.second + 12; ++cc) {
            if (rr < 0 || rr >= MSZ || cc < 0 || cc >= MSZ) continue;
            if (BlocksMovement(map[rr][cc])) continue;

            int dist = std::abs(rr - tgt.first) + std::abs(cc - tgt.second);
            if (dist > WarriorNPC::GetFireRange()) continue; // must be within rifle range

            if (!LineOfFireWS(map, rr, cc, tgt.first, tgt.second)) continue;

            double cost = smap[rr][cc]; // prefer safer cells
            if (cost < bestCost) { bestCost = cost; bestR = rr; bestC = cc; }
        }
    }

    int destR = (bestR == -1 ? tgt.first : bestR);
    int destC = (bestC == -1 ? tgt.second : bestC);

    auto path = RiskAwareAStar(map, smap, w->Row(), w->Col(), destR, destC, w->GetRiskWeight());
    if (!path.empty()) { w->SetPathCells(path); w->setIsMoving(true); }
    else { w->setIsMoving(false); }
}
void WarriorAttack::Transition(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    auto tgt = w->GetMissionTarget();
    int dist = std::abs(w->Row() - tgt.first) + std::abs(w->Col() - tgt.second);

    if (dist <= 2) {
        if (w->CanSeeTarget(tgt.first, tgt.second, w->EnvMap())) {
            w->FireAtTarget(tgt.first, tgt.second);
        }
    }
    if (dist <= 1 || w->Ammo() == 0) {
        OnExit(np);
        w->setCurrentState(new WarriorIdle());
        w->getCurrentState()->OnEnter(w);
    }
}
void WarriorAttack::OnExit(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    w->setIsMoving(false);
}

// ===== WarriorDefend =====
void WarriorDefend::OnEnter(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    if (!w->EnvMap() || !w->EnvSMap()) { w->setIsMoving(false); return; }

    auto safe = w->GetMissionTarget();
    if (safe.first == -1) { w->setIsMoving(false); return; }
    if (safe.first == w->Row() && safe.second == w->Col()) { w->setIsMoving(false); return; }

    auto path = RiskAwareAStar(w->EnvMap(), w->EnvSMap(),
        w->Row(), w->Col(), safe.first, safe.second,
        w->GetRiskWeight() * 1.5);
    if (!path.empty()) { w->SetPathCells(path); w->setIsMoving(true); }
    else { w->setIsMoving(false); }
}
void WarriorDefend::Transition(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    auto tgt = w->GetMissionTarget();
    int dist = std::abs(w->Row() - tgt.first) + std::abs(w->Col() - tgt.second);
    if (dist <= 1) {
        OnExit(np);
        w->setCurrentState(new WarriorIdle());
        w->getCurrentState()->OnEnter(w);
    }
}
void WarriorDefend::OnExit(NPC* np) {
    auto* w = dynamic_cast<WarriorNPC*>(np);
    assert(w);
    w->setIsMoving(false);
}
