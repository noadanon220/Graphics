// ============== MovingState.h ==============
#ifndef MOVINGSTATE_H
#define MOVINGSTATE_H

#include "State.h"
#include "Position.h"

class MovingState : public State {
private:
    Position destination;

public:
    MovingState(Position dest) : destination(dest) {}

    void OnEnter(Character* character) override;
    void Update(Character* character) override;
    void OnExit(Character* character) override;
};

#endif