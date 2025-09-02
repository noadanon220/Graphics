//MedicNPC.cpp
#include "MedicNPC.h"
#include "WarriorNPC.h"
#include "AStar.h"
#include <cassert>
#include <iostream>

static void PlanOrIdle(MedicNPC* m, int (*map)[MSZ], double (*smap)[MSZ], std::pair<int, int> goal) {
    auto path = RiskAwareAStar(map, smap, m->Row(), m->Col(), goal.first, goal.second, m->RiskW());
    if (path.empty()) {
        m->setIsMoving(false);
    }
    else {
        m->SetPathCells(path);
        m->setIsMoving(true);
    }
}

void MedicGoToDepot::OnEnter(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    m->setIsMoving(true);
    if (m->EnvMap() && m->EnvSMap()) {
        PlanOrIdle(m, m->EnvMap(), m->EnvSMap(), m->Depot());
    }
}
void MedicGoToDepot::Transition(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    auto* next = new MedicGoToTarget();
    m->setCurrentState(next);
    next->OnEnter(m);
}
void MedicGoToDepot::OnExit(NPC*) {}

void MedicGoToTarget::OnEnter(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    m->setIsMoving(true);
    if (m->EnvMap() && m->EnvSMap())
        PlanOrIdle(m, m->EnvMap(), m->EnvSMap(), m->Target());
}
void MedicGoToTarget::Transition(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    // Arrived: heal target and go idle
    if (auto* w = m->TargetWarrior()) {
        w->HealFull();
    }
    auto* idle = new MedicIdle();
    m->setCurrentState(idle);
    idle->OnEnter(m);
}
void MedicGoToTarget::OnExit(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    m->setIsMoving(false);
}

void MedicIdle::OnEnter(NPC* np) { (void)np; }
void MedicIdle::Transition(NPC* np) { (void)np; }
void MedicIdle::OnExit(NPC* np) { (void)np; }

void MedicNPC::SetMission(int depotR, int depotC, int targetR, int targetC,
    double riskWeight, WarriorNPC* wounded) {
    depot = { depotR,depotC };
    target = { targetR,targetC };
    riskW = riskWeight;
    woundedPtr = wounded;
}

std::pair<int, int> MedicNPC::Depot()  const { return depot; }
std::pair<int, int> MedicNPC::Target() const { return target; }
double MedicNPC::RiskW() const { return riskW; }
