#include "MedicNPC.h"
#include "AStar.h"
#include <cassert>
#include <iostream> // for logging

static void PlanOrIdle(MedicNPC* m, int (*map)[MSZ], double (*smap)[MSZ], std::pair<int, int> goal) {
    std::cout << "M: PlanOrIdle - planning path to (" << goal.first << "," << goal.second << ")" << std::endl;
    auto path = RiskAwareAStar(map, smap, m->Row(), m->Col(), goal.first, goal.second, m->RiskW());
    if (path.empty()) {
        std::cout << "M: PlanOrIdle - no path found, staying idle" << std::endl;
        m->setIsMoving(false);
    }
    else {
        std::cout << "M: PlanOrIdle - path found with " << path.size() << " waypoints" << std::endl;
        m->SetPathCells(path);
        m->setIsMoving(true);
    }
}

void MedicGoToDepot::OnEnter(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    std::cout << "M: going to depot (" << m->Depot().first << "," << m->Depot().second << ")" << std::endl;
    m->setIsMoving(true);
    if (m->EnvMap() && m->EnvSMap()) {
        PlanOrIdle(m, m->EnvMap(), m->EnvSMap(), m->Depot());
    }
}
void MedicGoToDepot::Transition(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    std::cout << "M: MedicGoToDepot::Transition - arrived at depot, transitioning to target" << std::endl;
    // Arrived: next leg to target
    auto* next = new MedicGoToTarget();
    m->setCurrentState(next);
    next->OnEnter(m);
}
void MedicGoToDepot::OnExit(NPC*) {
    std::cout << "M: MedicGoToDepot::OnExit" << std::endl;
}

void MedicGoToTarget::OnEnter(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    std::cout << "M: going to target (" << m->Target().first << "," << m->Target().second << ")" << std::endl;
    m->setIsMoving(true);
    if (m->EnvMap() && m->EnvSMap())
        PlanOrIdle(m, m->EnvMap(), m->EnvSMap(), m->Target());
}
void MedicGoToTarget::Transition(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    std::cout << "M: MedicGoToTarget::Transition - arrived at target, healing soldier and going idle" << std::endl;
    // Arrived: heal target and go idle
    // Real healing is done by commander when mission assigned
    auto* idle = new MedicIdle();
    m->setCurrentState(idle);
    idle->OnEnter(m);
}
void MedicGoToTarget::OnExit(NPC* np) {
    auto* m = dynamic_cast<MedicNPC*>(np);
    assert(m);
    std::cout << "M: MedicGoToTarget::OnExit - stopping movement" << std::endl;
    m->setIsMoving(false);
}

void MedicIdle::OnEnter(NPC* np) { 
    std::cout << "M: entering IDLE state" << std::endl;
}

void MedicIdle::Transition(NPC* np) {} 

void MedicIdle::OnExit(NPC* np) {
    std::cout << "M: leaving IDLE state" << std::endl;
}

void MedicNPC::SetMission(int depotR, int depotC, int targetR, int targetC, double riskWeight) {
    std::cout << "M: SetMission - depot (" << depotR << "," << depotC << ") -> target (" 
              << targetR << "," << targetC << ") with risk weight " << riskWeight << std::endl;
    depot = { depotR,depotC };
    target = { targetR,targetC };
    riskW = riskWeight;
}

std::pair<int, int> MedicNPC::Depot()  const { 
    static int depotCount = 0;
    if (++depotCount % 200 == 0) { // Log every 200 calls
        std::cout << "M: Depot() called, returning (" << depot.first << "," << depot.second << ")" << std::endl;
    }
    return depot; 
}

std::pair<int, int> MedicNPC::Target() const { 
    static int targetCount = 0;
    if (++targetCount % 200 == 0) { // Log every 200 calls
        std::cout << "M: Target() called, returning (" << target.first << "," << target.second << ")" << std::endl;
    }
    return target; 
}

double MedicNPC::RiskW() const { 
    static int riskWCount = 0;
    if (++riskWCount % 200 == 0) { // Log every 200 calls
        std::cout << "M: RiskW() called, returning " << riskW << std::endl;
    }
    return riskW; 
}


