// ============== Commander.cpp ==============
#include "Commander.h"
#include "MovingState.h"
#include <iostream>

Commander::Commander(Position p, int team) : Character(p, team, CharacterType::COMMANDER) {
    currentStrategy = StrategyType::DEFEND;
    lastOrderTime = 0;

    // Initialize combined visibility
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            combinedVisibility[i][j] = false;
        }
    }
}

void Commander::Update(GameMap* map) {
    UpdateVisibilityMap(map);
    UpdateCombinedVisibility();

    // Update FSM
    currentState->Update(this);

    // AI decision making every few seconds
    static int frameCounter = 0;
    frameCounter++;

    if (frameCounter % 180 == 0) { // Every 3 seconds at 60 FPS
        AnalyzeSituation();
        PlanStrategy();
        GiveOrders();
    }

    // Move to safer position if under threat
    if (health < maxHealth * 0.7) {
        // Find safe position (simplified)
        Position safePos = FindSafePosition(map);
        if (Distance(pos, safePos) > 1.0) {
            setState(new MovingState(safePos));
        }
    }
}

void Commander::Show() {
    // Team color
    if (team == 0) {
        glColor3d(0.0, 0.0, 1.0); // blue
    }
    else {
        glColor3d(1.0, 0.5, 0.0); // orange
    }

    // Draw character body
    glBegin(GL_POLYGON);
    glVertex2d(pos.x - 0.4, pos.y - 0.4);
    glVertex2d(pos.x + 0.4, pos.y - 0.4);
    glVertex2d(pos.x + 0.4, pos.y + 0.4);
    glVertex2d(pos.x - 0.4, pos.y + 0.4);
    glEnd();

    // Draw character symbol
    glColor3d(1.0, 1.0, 1.0); // white
    glRasterPos2d(pos.x - 0.1, pos.y + 0.1);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'C');

    // Health bar
    if (health < maxHealth) {
        glColor3d(0, 0, 0);
        glBegin(GL_POLYGON);
        glVertex2d(pos.x - 0.4, pos.y + 0.5);
        glVertex2d(pos.x + 0.4, pos.y + 0.5);
        glVertex2d(pos.x + 0.4, pos.y + 0.6);
        glVertex2d(pos.x - 0.4, pos.y + 0.6);
        glEnd();

        double healthRatio = (double)health / maxHealth;
        if (healthRatio > 0.6) glColor3d(0, 1, 0);
        else if (healthRatio > 0.3) glColor3d(1, 1, 0);
        else glColor3d(1, 0, 0);

        glBegin(GL_POLYGON);
        glVertex2d(pos.x - 0.4, pos.y + 0.5);
        glVertex2d(pos.x - 0.4 + 0.8 * healthRatio, pos.y + 0.5);
        glVertex2d(pos.x - 0.4 + 0.8 * healthRatio, pos.y + 0.6);
        glVertex2d(pos.x - 0.4, pos.y + 0.6);
        glEnd();
    }

    // Show strategy indicator
    glColor3d(1.0, 1.0, 1.0);
    glRasterPos2d(pos.x - 0.4, pos.y - 0.7);
    char strategyChar;
    switch (currentStrategy) {
    case StrategyType::ATTACK:
        strategyChar = 'A';
        break;
    case StrategyType::DEFEND:
        strategyChar = 'D';
        break;
    case StrategyType::REGROUP:
        strategyChar = 'R';
        break;
    default:
        strategyChar = '?';
    }
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, strategyChar);
}

void Commander::UpdateCombinedVisibility() {
    // Reset combined visibility
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            combinedVisibility[i][j] = false;
        }
    }

    // Add commander's own visibility
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (visibilityMap[i][j]) {
                combinedVisibility[i][j] = true;
            }
        }
    }

    // Combine team members' visibility
    for (Character* member : teamMembers) {
        if (member->isAlive()) {
            for (int i = 0; i < MAP_SIZE; i++) {
                for (int j = 0; j < MAP_SIZE; j++) {
                    if (member->getVisibility(j, i)) {
                        combinedVisibility[i][j] = true;
                    }
                }
            }
        }
    }
}

void Commander::AnalyzeSituation() {
    // Count team members' status
    int aliveMembers = 0;
    int healthyMembers = 0;
    int woundedMembers = 0;
    int warriorsLowAmmo = 0;

    for (Character* member : teamMembers) {
        if (member->isAlive()) {
            aliveMembers++;
            if (member->getHealth() > 70) {
                healthyMembers++;
            }
            else if (member->getHealth() < 30) {
                woundedMembers++;
            }

            // Check if warrior needs ammo (simplified check)
            if (member->getType() == CharacterType::WARRIOR) {
                // In a real implementation, we'd check ammo levels
                if (rand() % 10 < 2) { // 20% chance warrior needs ammo
                    warriorsLowAmmo++;
                }
            }
        }
    }

    // Count visible enemies
    int visibleEnemies = 0;
    int enemyThreats = 0;

    // This would be implemented with access to enemy team data
    // For now, simulate enemy analysis
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (combinedVisibility[i][j]) {
                // Simulate enemy detection
                if (rand() % 100 < 5) { // 5% chance of enemy in visible area
                    visibleEnemies++;
                    if (Distance(pos, Position(j + 0.5, i + 0.5)) < 10.0) {
                        enemyThreats++;
                    }
                }
            }
        }
    }

    // Store analysis results for strategy planning
    lastAnalysis.aliveMembers = aliveMembers;
    lastAnalysis.healthyMembers = healthyMembers;
    lastAnalysis.woundedMembers = woundedMembers;
    lastAnalysis.visibleEnemies = visibleEnemies;
    lastAnalysis.enemyThreats = enemyThreats;
}

void Commander::PlanStrategy() {
    StrategyType newStrategy = currentStrategy;

    // Strategy decision logic
    if (lastAnalysis.woundedMembers > 2 || lastAnalysis.aliveMembers < 3) {
        // Too many casualties - regroup and defend
        newStrategy = StrategyType::REGROUP;
    }
    else if (lastAnalysis.healthyMembers >= 4 && lastAnalysis.visibleEnemies > 0) {
        // Strong team with visible enemies - attack
        newStrategy = StrategyType::ATTACK;
    }
    else if (lastAnalysis.enemyThreats > 0) {
        // Enemies nearby but not strong enough to attack - defend
        newStrategy = StrategyType::DEFEND;
    }
    else {
        // Default to defensive posture
        newStrategy = StrategyType::DEFEND;
    }

    // Change strategy if different
    if (newStrategy != currentStrategy) {
        currentStrategy = newStrategy;

        std::cout << "Team " << team << " Commander: Strategy changed to ";
        switch (currentStrategy) {
        case StrategyType::ATTACK:
            std::cout << "ATTACK" << std::endl;
            break;
        case StrategyType::DEFEND:
            std::cout << "DEFEND" << std::endl;
            break;
        case StrategyType::REGROUP:
            std::cout << "REGROUP" << std::endl;
            break;
        }
    }
}

void Commander::GiveOrders() {
    // Issue orders to team members based on current strategy
    // This is a simplified version - in a full implementation,
    // orders would be sent to specific team members

    switch (currentStrategy) {
    case StrategyType::ATTACK:
        GiveAttackOrders();
        break;
    case StrategyType::DEFEND:
        GiveDefendOrders();
        break;
    case StrategyType::REGROUP:
        GiveRegroupOrders();
        break;
    }

    // Check if medical or supply assistance is needed
    if (lastAnalysis.woundedMembers > 0) {
        OrderMedicalAssistance();
    }

    lastOrderTime++;
}

void Commander::GiveAttackOrders() {
    // Find best attack positions based on combined visibility
    std::vector<Position> attackPositions;

    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            if (combinedVisibility[i][j]) {
                // Look for good attack positions
                attackPositions.push_back(Position(j + 0.5, i + 0.5));
            }
        }
    }

    // In a full implementation, specific warriors would be given
    // specific positions to attack from
    if (!attackPositions.empty()) {
        std::cout << "Team " << team << " Commander: Issuing attack orders to "
            << attackPositions.size() << " positions" << std::endl;
    }
}

void Commander::GiveDefendOrders() {
    // Find good defensive positions
    std::vector<Position> defendPositions;

    // Look for positions behind cover
    for (int i = 0; i < MAP_SIZE; i++) {
        for (int j = 0; j < MAP_SIZE; j++) {
            // Look for positions near rocks for cover
            if (combinedVisibility[i][j]) {
                defendPositions.push_back(Position(j + 0.5, i + 0.5));
            }
        }
    }

    std::cout << "Team " << team << " Commander: Issuing defensive orders" << std::endl;
}

void Commander::GiveRegroupOrders() {
    // Order team to fall back to safe positions
    Position regroupPoint = FindRegroupPosition();

    std::cout << "Team " << team << " Commander: Ordering regroup at ("
        << regroupPoint.x << ", " << regroupPoint.y << ")" << std::endl;
}

void Commander::OrderMedicalAssistance() {
    // Find medic and order them to help wounded
    for (Character* member : teamMembers) {
        if (member->isAlive() && member->getType() == CharacterType::MEDIC) {
            std::cout << "Team " << team << " Commander: Ordering medical assistance" << std::endl;
            break;
        }
    }
}

Position Commander::FindSafePosition(GameMap* map) {
    // Find a safe position for the commander
    Position safest = pos;
    double bestSafety = 0;

    // Search in a radius around current position
    for (int dx = -5; dx <= 5; dx++) {
        for (int dy = -5; dy <= 5; dy++) {
            int checkX = (int)pos.x + dx;
            int checkY = (int)pos.y + dy;

            if (map->isInBounds(checkX, checkY) && map->isPassable(checkX, checkY)) {
                Position candidate(checkX + 0.5, checkY + 0.5);
                double safety = CalculatePositionSafety(candidate, map);

                if (safety > bestSafety) {
                    bestSafety = safety;
                    safest = candidate;
                }
            }
        }
    }

    return safest;
}

Position Commander::FindRegroupPosition() {
    // Find a central, safe position for regrouping
    Position regroup = pos;

    // Simple implementation - move towards team spawn area
    if (team == 0) {
        regroup = Position(8, 8); // Team 0 regroup point
    }
    else {
        regroup = Position(MAP_SIZE - 8, MAP_SIZE - 8); // Team 1 regroup point
    }

    return regroup;
}

double Commander::CalculatePositionSafety(Position candidate, GameMap* map) {
    double safety = 10.0; // Base safety score

    // Penalty for being too far from team
    double teamDistance = 0;
    int teamCount = 0;
    for (Character* member : teamMembers) {
        if (member->isAlive()) {
            teamDistance += Distance(candidate, member->getPosition());
            teamCount++;
        }
    }
    if (teamCount > 0) {
        double avgTeamDistance = teamDistance / teamCount;
        safety -= avgTeamDistance * 0.5; // Penalty for being far from team
    }

    // Bonus for being near cover
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            int checkX = (int)candidate.x + dx;
            int checkY = (int)candidate.y + dy;

            if (map->isInBounds(checkX, checkY)) {
                if (map->getTerrain(checkX, checkY) == TerrainType::ROCK) {
                    safety += 2.0; // Bonus for nearby cover
                }
            }
        }
    }

    return safety;
}