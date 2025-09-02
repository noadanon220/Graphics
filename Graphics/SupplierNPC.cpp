#include "SupplierNPC.h"
#include "AStar.h"
#include <cassert>
#include <iostream> // for logging

static void PlanOrIdleS(SupplierNPC* s, int (*map)[MSZ], double (*smap)[MSZ], std::pair<int, int> goal) {
    std::cout << "P: PlanOrIdleS - planning path to (" << goal.first << "," << goal.second << ")" << std::endl;
    auto path = RiskAwareAStar(map, smap, s->Row(), s->Col(), goal.first, goal.second, s->RiskW());
    if (path.empty()) {
        std::cout << "P: PlanOrIdleS - no path found, staying idle" << std::endl;
        s->setIsMoving(false);
    }
    else {
        std::cout << "P: PlanOrIdleS - path found with " << path.size() << " waypoints" << std::endl;
        s->SetPathCells(path);
        s->setIsMoving(true);
    }
}

void SupplierNPC::SetMission(int depotR, int depotC, int targetR, int targetC, double riskWeight) {
    std::cout << "P: SetMission - depot (" << depotR << "," << depotC << ") -> target (" 
              << targetR << "," << targetC << ") with risk weight " << riskWeight << std::endl;
    depot = { depotR,depotC };
    target = { targetR,targetC };
    riskW = riskWeight;
}

std::pair<int, int> SupplierNPC::Depot()  const { 
    static int depotCount = 0;
    if (++depotCount % 200 == 0) { // Log every 200 calls
        std::cout << "P: Depot() called, returning (" << depot.first << "," << depot.second << ")" << std::endl;
    }
    return depot; 
}

std::pair<int, int> SupplierNPC::Target() const { 
    static int targetCount = 0;
    if (++targetCount % 200 == 0) { // Log every 200 calls
        std::cout << "P: Target() called, returning (" << target.first << "," << target.second << ")" << std::endl;
    }
    return target; 
}

double SupplierNPC::RiskW() const { 
    static int riskWCount = 0;
    if (++riskWCount % 200 == 0) { // Log every 200 calls
        std::cout << "P: RiskW() called, returning " << riskW << std::endl;
    }
    return riskW; 
}

void SupplierGoToDepot::OnEnter(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    std::cout << "P: going to depot (" << s->Depot().first << "," << s->Depot().second << ")" << std::endl;
    s->setIsMoving(true);
    if (s->EnvMap() && s->EnvSMap())
        PlanOrIdleS(s, s->EnvMap(), s->EnvSMap(), s->Depot());
}
void SupplierGoToDepot::Transition(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    std::cout << "P: SupplierGoToDepot::Transition - arrived at depot, transitioning to target" << std::endl;
    auto* next = new SupplierGoToTarget();
    s->setCurrentState(next);
    next->OnEnter(s);
}
void SupplierGoToDepot::OnExit(NPC* np) { 
    std::cout << "P: SupplierGoToDepot::OnExit" << std::endl;
    (void)np; 
}

void SupplierGoToTarget::OnEnter(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    std::cout << "P: going to target (" << s->Target().first << "," << s->Target().second << ")" << std::endl;
    s->setIsMoving(true);
    if (s->EnvMap() && s->EnvSMap())
        PlanOrIdleS(s, s->EnvMap(), s->EnvSMap(), s->Target());
}
void SupplierGoToTarget::Transition(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    std::cout << "P: SupplierGoToTarget::Transition - arrived at target, supplying ammo and going idle" << std::endl;
    auto* idle = new SupplierIdle();
    s->setCurrentState(idle);
    idle->OnEnter(s);
}
void SupplierGoToTarget::OnExit(NPC* np) {
    auto* s = dynamic_cast<SupplierNPC*>(np);
    assert(s);
    std::cout << "P: SupplierGoToTarget::OnExit - stopping movement" << std::endl;
    s->setIsMoving(false);
}

void SupplierIdle::OnEnter(NPC* np) { 
    std::cout << "P: entering IDLE state" << std::endl;
}

void SupplierIdle::Transition(NPC* np) {} 

void SupplierIdle::OnExit(NPC* np) {
    std::cout << "P: leaving IDLE state" << std::endl;
}


