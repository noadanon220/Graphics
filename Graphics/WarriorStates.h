// WarriorStates.h
#pragma once
#include "NPC.h"

// מצבי הלוחם:
// שימי לב: State היא מחלקת בסיס שמוגדרת ב-NPC.h (OnEnter/Transition/OnExit ווירטואליות).

class WarriorIdle : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};

class WarriorMoveTo : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};

class WarriorAttack : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};

class WarriorDefend : public State {
public:
    void OnEnter(NPC* np) override;
    void Transition(NPC* np) override;
    void OnExit(NPC* np) override;
};
