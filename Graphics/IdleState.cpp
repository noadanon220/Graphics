// ============== IdleState.cpp ==============
#include "IdleState.h"
#include "Character.h"

void IdleState::OnEnter(Character* character) {
    // Character enters idle state
}

void IdleState::Update(Character* character) {
    // Update visibility and wait for orders
    // This will be overridden by specific character AI
}

void IdleState::OnExit(Character* character) {
    // Character exits idle state
}
