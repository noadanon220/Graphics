// ============== Supplier.h ==============
#ifndef SUPPLIER_H
#define SUPPLIER_H

#include "Character.h"
#include "Warrior.h"

enum class SupplierState {
    IDLE,
    MOVING_TO_SUPPLIES,
    MOVING_TO_WARRIORS,
    SUPPLYING
};

class Supplier : public Character {
private:
    SupplierState supplierState;
    std::vector<Warrior*> targetWarriors;
    Position ammoSupplyPos;
    bool hasAmmo;
    int ammoAmount;

public:
    Supplier(Position p, int team, Position ammoPos);

    void Update(GameMap* map) override;
    void Show() override;

    void ReceiveSupplyOrder(std::vector<Warrior*> warriors);
    void SupplyAmmo();
    void CollectAmmo();

    bool canSupply() const { return hasAmmo; }
};

#endif