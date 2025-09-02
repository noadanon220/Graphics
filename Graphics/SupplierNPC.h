//SupplierNPC.h
#pragma once
#include "NPC.h"

// Forward-declare WarriorNPC to avoid heavy includes
class WarriorNPC;

// Forward-declared states
class SupplierGoToDepot;
class SupplierGoToTarget;
class SupplierIdle;

// Supply soldier: fetches ammo then delivers to a warrior
class SupplierNPC : public NPC {
public:
    explicit SupplierNPC(Team t) : NPC(t, SUP) {}

    void SetMission(int depotR, int depotC, int targetR, int targetC,
        double riskWeight, WarriorNPC* receiver);
    std::pair<int, int> Depot()  const;
    std::pair<int, int> Target() const;
    double RiskW() const;
    WarriorNPC* TargetWarrior() const { return receiverPtr; }

private:
    std::pair<int, int> depot{ -1,-1 };
    std::pair<int, int> target{ -1,-1 };
    double riskW = 10.0;
    WarriorNPC* receiverPtr = nullptr;
};

// Supply states
class SupplierGoToDepot : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};
class SupplierGoToTarget : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};
class SupplierIdle : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};
