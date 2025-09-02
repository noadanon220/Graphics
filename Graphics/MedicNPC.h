//MedicNPC.h
#pragma once
#include "NPC.h"

// Forward-declare WarriorNPC to avoid heavy includes
class WarriorNPC;

// Forward-declare states
class MedicGoToDepot;
class MedicGoToTarget;
class MedicIdle;

// Medic character: go to depot then heal wounded teammate
class MedicNPC : public NPC {
public:
    explicit MedicNPC(Team t) : NPC(t, MED) {}

    void SetMission(int depotR, int depotC, int targetR, int targetC,
        double riskWeight, WarriorNPC* wounded);
    std::pair<int, int> Depot()  const;
    std::pair<int, int> Target() const;
    double RiskW() const;
    WarriorNPC* TargetWarrior() const { return woundedPtr; }

private:
    std::pair<int, int> depot{ -1,-1 };
    std::pair<int, int> target{ -1,-1 };
    double riskW = 10.0;
    WarriorNPC* woundedPtr = nullptr;
};

// States
class MedicGoToDepot : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};
class MedicGoToTarget : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};
class MedicIdle : public State {
public:
    void OnEnter(NPC*) override;
    void Transition(NPC*) override;
    void OnExit(NPC*) override;
};
