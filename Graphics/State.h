#pragma once

class NPC;

// Base state class for finite state machines
class State {
public:
    virtual ~State() = default;
    virtual void OnEnter(NPC* np) = 0;     // Called when state becomes active
    virtual void Transition(NPC* np) = 0;  // Called when state should switch
    virtual void OnExit(NPC* np) = 0;      // Called just before leaving state
};
