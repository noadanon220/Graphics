//SupplierNPC.cpp
#include "SupplierNPC.h"
#include "WarriorNPC.h"
#include "AStar.h"
#include <cassert>
#include <iostream>

static void PlanOrIdleS(SupplierNPC* s, int (*map)[MSZ], double (*smap)[MSZ], std::pair<int, int> goal) {
    auto path = RiskAwareAStar(map, smap, s->Row(), s->Col(), goal.first, goal.second, s->RiskW());
    if (path.empty()) {
        s->setIsMoving(false);
    }
    else {
        s->SetPathCells(path);
        s->setIsMoving(true);
    }
}

void SupplierNPC::SetMission(int depotR, int depotC, int targetR, int targetC,
    double riskWeight, WarriorNPC* receiver) {
    depot = { depotR,depotC };
    target = { targetR,targetC };
    riskW = riskWeight;
    receiverPtr = receiver;
}

std::pair<int, int> SupplierNPC::Depot()  const { return depot; }
std::pair<int, int> SupplierNPC::Target() const { return target; }
double SupplierNPC::RiskW() const { return riskW; }

void SupplierGoToDepot::OnEnter(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    s->setIsMoving(true);
    if (s->EnvMap() && s->EnvSMap())
        PlanOrIdleS(s, s->EnvMap(), s->EnvSMap(), s->Depot());
}
void SupplierGoToDepot::Transition(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    auto* next = new SupplierGoToTarget();
    s->setCurrentState(next);
    next->OnEnter(s);
}
void SupplierGoToDepot::OnExit(NPC* np) { (void)np; }

void SupplierGoToTarget::OnEnter(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    s->setIsMoving(true);
    if (s->EnvMap() && s->EnvSMap())
        PlanOrIdleS(s, s->EnvMap(), s->EnvSMap(), s->Target());
}
void SupplierGoToTarget::Transition(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    if (auto* w = s->TargetWarrior()) {
        w->RefillAmmo();
    }
    auto* idle = new SupplierIdle();
    s->setCurrentState(idle);
    idle->OnEnter(s);
}
void SupplierGoToTarget::OnExit(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    s->setIsMoving(false);
}

void SupplierIdle::OnEnter(NPC* np) { (void)np; }
void SupplierIdle::Transition(NPC* np) { (void)np; }
void SupplierIdle::OnExit(NPC* np) { (void)np; }
