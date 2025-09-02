// ============== State.h ==============
#ifndef STATE_H
#define STATE_H

class Character;

class State {
public:
    virtual ~State() {}
    virtual void OnEnter(Character* character) = 0;
    virtual void Update(Character* character) = 0;
    virtual void OnExit(Character* character) = 0;
};

#endif