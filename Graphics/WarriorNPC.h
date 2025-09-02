#pragma once
#include "NPC.h"
#include <algorithm>


// Forward declarations for state classes
class WarriorMoveTo;
class WarriorAttack;
class WarriorDefend;
class WarriorIdle;

// Warrior NPC: has health, ammo, grenades, and missions.
class WarriorNPC : public NPC {
public:
    enum MissionType { MOVE_TO, ATTACK, DEFEND, IDLE };
    explicit WarriorNPC(Team t) : NPC(t, WAR) {}

    // Health/ammo interface
    void Damage(int d);
    bool IsAlive() const;
    bool IsWounded() const;
    bool AmmoLow() const;
    void ConsumeAmmo(int k);
    int  HP()   const;
    int  Ammo() const;

    // Mission commands
    void SetMission(int targetR, int targetC, MissionType type, double rw = 10.0);
    std::pair<int, int> GetMissionTarget() const;
    MissionType GetMissionType() const;
    double GetRiskWeight() const;

    // Combat functions
    bool CanSeeTarget(int targetR, int targetC, int map[MSZ][MSZ]) const;
    bool InFireRange(int targetR, int targetC) const;
    bool InGrenadeRange(int targetR, int targetC) const;
    void FireAtTarget(int targetR, int targetC);
    void ThrowGrenadeAt(int targetR, int targetC);

    // Debug/test hooks
    void TestDamage();
    void TestLowAmmo();

private:
    static constexpr int MAX_HP = 100;
    static constexpr int AMMO_LOW = 5;
    static constexpr int FIRE_RANGE = 8;
    static constexpr int GRENADE_RANGE = 6;
    int hp = MAX_HP;
    int ammo = 20;
    int grenades = 3;
    bool alive = true;

    std::pair<int, int> missionTarget{ -1,-1 };
    MissionType missionType = IDLE;
    double riskWeight = 10.0;
};
