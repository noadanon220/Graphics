// ============== MovingState.cpp ==============
#include "MovingState.h"
#include "Character.h"
#include "IdleState.h"

void MovingState::OnEnter(Character* character) {
    character->MoveTo(destination);
}

void MovingState::Update(Character* character) {
    character->UpdateMovement();

    // Check if reached destination
    if (Distance(character->getPosition(), destination) < 0.2) {
        character->setState(new IdleState());
    }
}

void MovingState::OnExit(Character* character) {
    // Movement complete
}
