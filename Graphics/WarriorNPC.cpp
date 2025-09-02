// WarriorNPC.cpp
#include "WarriorNPC.h"
#include "Definitions.h"
#include <cmath>
#include <iostream>

// Bresenham line-of-fire; stops at firing obstacles
static bool LineOfFire(int map[MSZ][MSZ], int r0, int c0, int r1, int c1) {
    int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
    int sr = (r0 < r1) ? 1 : -1;
    int sc = (c0 < c1) ? 1 : -1;
    int err = dc - dr;
    int r = r0, c = c0;
    while (!(r == r1 && c == c1)) {
        int e2 = 2 * err;
        if (e2 > -dr) { err -= dr; c += sc; }
        if (e2 < dc) { err += dc; r += sr; }
        if (r == r1 && c == c1) break;
        if (r < 0 || r >= MSZ || c < 0 || c >= MSZ) return false;
        if (BlocksFire(map[r][c])) return false;
    }
    return true;
}

bool WarriorNPC::CanSeeTarget(int targetR, int targetC, int map[MSZ][MSZ]) const {
    int md = std::abs(Row() - targetR) + std::abs(Col() - targetC);
    if (md > FIRE_RANGE * 2) return false;
    return LineOfFire(map, Row(), Col(), targetR, targetC);
}

bool WarriorNPC::InFireRange(int targetR, int targetC) const {
    return std::abs(Row() - targetR) + std::abs(Col() - targetC) <= FIRE_RANGE;
}

bool WarriorNPC::InGrenadeRange(int targetR, int targetC) const {
    return grenades > 0 &&
        (std::abs(Row() - targetR) + std::abs(Col() - targetC) <= GRENADE_RANGE);
}

void WarriorNPC::FireAtTarget(int targetR, int targetC) {
    if (ammo <= 0) return;
    if (InFireRange(targetR, targetC)) {
        ammo = std::max(0, ammo - 1);
        std::cout << "Warrior FIRE at (" << targetR << "," << targetC << ") - Ammo: " << ammo << "\n";
    }
}

void WarriorNPC::ThrowGrenadeAt(int targetR, int targetC) {
    if (!InGrenadeRange(targetR, targetC)) return;
    grenades = std::max(0, grenades - 1);
    std::cout << "Warrior GRENADE at (" << targetR << "," << targetC << ") - Grenades: " << grenades << "\n";
}

bool WarriorNPC::IsWounded() const {
    return alive && hp < MAX_HP;
}

bool WarriorNPC::AmmoLow() const {
    return ammo < AMMO_LOW;
}

bool WarriorNPC::IsAlive() const {
    return alive;
}

int WarriorNPC::HP() const {
    return hp;
}

int WarriorNPC::Ammo() const {
    return ammo;
}

// Debug/test hooks
void WarriorNPC::TestDamage() {
    std::cout << "Warrior TestDamage: " << hp << " -> 30\n";
    hp = 30;
}

void WarriorNPC::TestLowAmmo() {
    std::cout << "Warrior TestLowAmmo: " << ammo << " -> 3\n";
    ammo = 3;
}

void WarriorNPC::Damage(int d) {
    hp = std::max(0, hp - d);
    if (hp == 0) {
        alive = false;
        std::cout << "Warrior killed!\n";
    }
}

void WarriorNPC::HealFull() {
    hp = MAX_HP;
    std::cout << "Warrior fully healed! HP: " << hp << "\n";
}

void WarriorNPC::RefillAmmo() {
    ammo = 20; // Reset to full ammo
    std::cout << "Warrior ammo refilled! Ammo: " << ammo << "\n";
}

bool WarriorNPC::CanFight() const {
    return alive && ammo > 0;
}

void WarriorNPC::ConsumeAmmo(int k) {
    ammo = std::max(0, ammo - k);
}

void WarriorNPC::SetMission(int targetR, int targetC, MissionType type, double rw) {
    missionTarget = { targetR, targetC };
    missionType = type;
    riskWeight = rw;
    std::cout << "Warrior SetMission: type " << type << " -> (" << targetR << "," << targetC << ") rw=" << rw << "\n";
}

std::pair<int, int> WarriorNPC::GetMissionTarget() const {
    return missionTarget;
}
