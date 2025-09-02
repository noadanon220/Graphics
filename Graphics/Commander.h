// ============== Commander.h ==============
#ifndef COMMANDER_H
#define COMMANDER_H

#include "Character.h"
#include <vector>


enum class StrategyType {
    ATTACK,
    DEFEND,
    REGROUP
};

class Commander : public Character {
private:
    bool combinedVisibility[MAP_SIZE][MAP_SIZE];
    StrategyType currentStrategy;
    std::vector<Character*> teamMembers;
    int lastOrderTime;

    // Analysis data structure
    struct SituationAnalysis {
        int aliveMembers;
        int healthyMembers;
        int woundedMembers;
        int visibleEnemies;
        int enemyThreats;
    } lastAnalysis;

    // Additional private methods
    void GiveAttackOrders();
    void GiveDefendOrders();
    void GiveRegroupOrders();
    void OrderMedicalAssistance();
    Position FindSafePosition(GameMap* map);
    Position FindRegroupPosition();
    double CalculatePositionSafety(Position candidate, GameMap* map);

public:
    Commander(Position p, int team);

    void Update(GameMap* map) override;
    void Show() override;

    void UpdateCombinedVisibility();
    void AnalyzeSituation();
    void PlanStrategy();
    void GiveOrders();
    void AddTeamMember(Character* member) { teamMembers.push_back(member); }

    StrategyType getStrategy() const { return currentStrategy; }
    bool canSeePosition(int x, int y) const { return combinedVisibility[y][x]; }
};

#endif
