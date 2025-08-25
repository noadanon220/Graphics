#pragma once
#include <vector>
#include <memory>
#include "Definitions.h"
#include "Agent.h"

// Owns and manages one team's squad (5 agents).
class TeamSquad {
public:
    explicit TeamSquad(Team id);

    // Build standard composition: 1C, 2W, 1M, 1S
    void CreateStandardSquad();

    // Place all members randomly in a region [r0..r1]x[c0..c1] on SPACE cells
    bool SpawnAllInRegion(int map[MSZ][MSZ], int r0, int r1, int c0, int c1);

    // Draw all agents
    void Draw() const;

    Team GetId() const { return teamId; }

private:
    Team teamId;
    std::vector<std::unique_ptr<Agent>> members;

    // Helpers
    static bool IsFreeForAgent(int map[MSZ][MSZ], int r, int c);
    static bool RandomFreeCellInRegion(int map[MSZ][MSZ],
        int r0, int r1, int c0, int c1,
        int& outR, int& outC, int maxTries = 2000);
};
