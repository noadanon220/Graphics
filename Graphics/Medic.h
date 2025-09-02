// ============== Medic.h ==============
#ifndef MEDIC_H
#define MEDIC_H

#include "Character.h"

enum class MedicState {
    IDLE,
    MOVING_TO_SUPPLIES,
    MOVING_TO_PATIENT,
    HEALING
};

class Medic : public Character {
private:
    MedicState medicState;
    Character* currentPatient;
    Position medSupplyPos;
    bool hasSupplies;
    int healAmount;

public:
    Medic(Position p, int team, Position medPos);

    void Update(GameMap* map) override;
    void Show() override;

    void ReceiveHealOrder(Character* patient);
    void HealPatient();
    void CollectSupplies();

    bool canHeal() const { return hasSupplies; }
};

#endif