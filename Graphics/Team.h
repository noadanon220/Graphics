// Team.h
#pragma once
#include <vector>
#include <memory>
#include <utility>
#include "Definitions.h"

class NPC;
class CommanderNPC;
class MedicNPC;
class SupplierNPC;
class WarriorNPC;

class TeamSquad {
public:
    explicit TeamSquad(Team id);
    Team GetId() const;

    void CreateStandardSquad();

    // Spawn all team members inside a sub-rectangle of the map
    bool SpawnAllInRegion(int map[MSZ][MSZ], int r0, int r1, int c0, int c1);

    // Attach shared environment pointers to all members
    void AttachEnvironmentToAll(int map[MSZ][MSZ], double smap[MSZ][MSZ]);

    // Per-frame rendering & update
    void Draw() const;
    void Update(int map[MSZ][MSZ], double smap[MSZ][MSZ]);

    // Commander intel
    void SetKnownEnemyDepots(const std::vector<std::pair<int, int>>& ammo,
        const std::vector<std::pair<int, int>>& med);
    void SetOwnDepots(const std::vector<std::pair<int, int>>& ammo,
        const std::vector<std::pair<int, int>>& med);

    // Helpers / accessors
    std::vector<WarriorNPC*> GetWarriors();
    CommanderNPC* GetCommander();
    MedicNPC* GetMedic();
    SupplierNPC* GetSupplier();
    bool GetFirstWarriorCell(int& outR, int& outC) const;

    // New: opponent and combat/sight API
    void SetOpponent(TeamSquad* op);
    std::vector<std::pair<int, int>> GetAlivePositions() const;
    void CollectSightings(int map[MSZ][MSZ]);
    const std::vector<std::pair<int, int>>& GetSightings() const { return lastSightings; }
    void ResolveCombat(int map[MSZ][MSZ], double smap[MSZ][MSZ]);

private:
    // Spawning helpers
    bool IsFreeForNPC(int map[MSZ][MSZ], int r, int c);
    bool RandomFreeCellInRegion(int map[MSZ][MSZ], int r0, int r1, int c0, int c1,
        int& outR, int& outC, int maxTries = 4000);

    // Commander “brain”
    void RecomputeTeamVisibility(int map[MSZ][MSZ]);
    std::pair<int, int> FindSafePositionBFS(int map[MSZ][MSZ], double smap[MSZ][MSZ],
        int startR, int startC, int searchRadius = 14);
    void CommanderBrainTick(int map[MSZ][MSZ], double smap[MSZ][MSZ]);

private:
    Team teamId;
    int  frameCounter = 0;

    std::vector<std::unique_ptr<NPC>> members;

    // Commander’s combined visibility
    double commanderVis[MSZ][MSZ] = { 0.0 };

    // Known depots
    std::vector<std::pair<int, int>> enemyAmmoDepots, enemyMedDepots;
    std::vector<std::pair<int, int>> ownAmmoDepots, ownMedDepots;

    // New: opponent, sightings (per tick)
    TeamSquad* opponent = nullptr;
    std::vector<std::pair<int, int>> lastSightings;
};
