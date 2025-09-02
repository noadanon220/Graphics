// ============== IdleState.h ==============
#ifndef IDLESTATE_H
#define IDLESTATE_H

#include "State.h"

class IdleState : public State {
public:
    void OnEnter(Character* character) override;
    void Update(Character* character) override;
    void OnExit(Character* character) override;
};

#endif
