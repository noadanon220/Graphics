// WarriorStates.cpp
#include "WarriorStates.h"
#include "WarriorNPC.h"
#include "AStar.h"
#include "Definitions.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

// ===== WarriorIdle =====
void WarriorIdle::OnEnter(NPC* np) {
    // אפשר לשים כאן לוג או כלום
}
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

    int attackR = tgt.first, attackC = tgt.second;
    for (int tries = 0; tries < 8; ++tries) {
        int offR = rand() % 7 - 3;
        int offC = rand() % 7 - 3;
        int rr = tgt.first + offR, cc = tgt.second + offC;
        if (rr >= 0 && rr < MSZ && cc >= 0 && cc < MSZ && !BlocksMovement(w->EnvMap()[rr][cc])) {
            attackR = rr; attackC = cc; break;
        }
    }
    auto path = RiskAwareAStar(w->EnvMap(), w->EnvSMap(),
        w->Row(), w->Col(), attackR, attackC,
        w->GetRiskWeight());
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
