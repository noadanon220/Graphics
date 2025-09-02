// WarriorNPC.cpp  (DROP-IN REPLACEMENT)
#include "WarriorNPC.h"
#include "Definitions.h"
#include <cmath>
#include <iostream> // for logging

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
        if (BlocksFire(map[r][c])) {
            static int blockedCount = 0;
            if (++blockedCount % 500 == 0) {
                std::cout << "LineOfFire: blocked at (" << r << "," << c
                    << ") by terrain " << map[r][c] << std::endl;
            }
            return false;
        }
    }
    return true;
}

bool WarriorNPC::CanSeeTarget(int targetR, int targetC, int map[MSZ][MSZ]) const {
    int md = std::abs(Row() - targetR) + std::abs(Col() - targetC);
    if (md > FIRE_RANGE * 2) return false;
    bool canSee = LineOfFire(map, Row(), Col(), targetR, targetC);
    static int canSeeCount = 0;
    if (++canSeeCount % 300 == 0) {
        std::cout << "Warrior CanSeeTarget (" << targetR << "," << targetC
            << "): " << (canSee ? "YES" : "NO")
            << " (distance: " << md << ", max: " << FIRE_RANGE * 2 << ")\n";
    }
    return canSee;
}

bool WarriorNPC::InFireRange(int targetR, int targetC) const {
    bool inRange = std::abs(Row() - targetR) + std::abs(Col() - targetC) <= FIRE_RANGE;
    static int fireRangeCount = 0;
    if (++fireRangeCount % 400 == 0) {
        std::cout << "Warrior InFireRange (" << targetR << "," << targetC
            << "): " << (inRange ? "YES" : "NO") << "\n";
    }
    return inRange;
}

bool WarriorNPC::InGrenadeRange(int targetR, int targetC) const {
    bool inRange = grenades > 0 &&
        (std::abs(Row() - targetR) + std::abs(Col() - targetC) <= GRENADE_RANGE);
    static int grenadeRangeCount = 0;
    if (++grenadeRangeCount % 400 == 0) {
        std::cout << "Warrior InGrenadeRange (" << targetR << "," << targetC
            << "): " << (inRange ? "YES" : "NO")
            << " (grenades: " << grenades << ")\n";
    }
    return inRange;
}

void WarriorNPC::FireAtTarget(int targetR, int targetC) {
    if (ammo <= 0) return;
    if (InFireRange(targetR, targetC)) {
        ammo = std::max(0, ammo - 1);
        std::cout << "Warrior FIRE at (" << targetR << "," << targetC
            << ") - Ammo: " << ammo << "\n";
        // Here you could spawn bullets and apply damage to enemies.
    }
}

void WarriorNPC::ThrowGrenadeAt(int targetR, int targetC) {
    if (!InGrenadeRange(targetR, targetC)) return;
    grenades = std::max(0, grenades - 1);
    std::cout << "Warrior GRENADE at (" << targetR << "," << targetC
        << ") - Grenades: " << grenades << "\n";
}

bool WarriorNPC::IsWounded() const {
    bool wounded = alive && hp < MAX_HP;
    static int woundedCount = 0;
    if (++woundedCount % 200 == 0) {
        std::cout << "Warrior IsWounded: " << (wounded ? "YES" : "NO")
            << " (HP: " << hp << "/" << MAX_HP << ")\n";
    }
    return wounded;
}

bool WarriorNPC::AmmoLow() const {
    bool lowAmmo = ammo < AMMO_LOW;
    static int ammoLowCount = 0;
    if (++ammoLowCount % 200 == 0) {
        std::cout << "Warrior AmmoLow: " << (lowAmmo ? "YES" : "NO")
            << " (Ammo: " << ammo << "/" << AMMO_LOW << ")\n";
    }
    return lowAmmo;
}

bool WarriorNPC::IsAlive() const {
    static int aliveCount = 0;
    if (++aliveCount % 500 == 0) {
        std::cout << "Warrior IsAlive: " << (alive ? "YES" : "NO") << "\n";
    }
    return alive;
}

int WarriorNPC::HP() const {
    static int hpCount = 0;
    if (++hpCount % 1000 == 0) {
        std::cout << "Warrior HP(): " << hp << "\n";
    }
    return hp;
}

int WarriorNPC::Ammo() const {
    static int ammoCount = 0;
    if (++ammoCount % 1000 == 0) {
        std::cout << "Warrior Ammo(): " << ammo << "\n";
    }
    return ammo;
}

double WarriorNPC::GetRiskWeight() const {
    static int getRiskCount = 0;
    if (++getRiskCount % 200 == 0) {
        std::cout << "Warrior GetRiskWeight: " << riskWeight << "\n";
    }
    return riskWeight;
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
    std::cout << "Warrior Damage: -" << d << " HP (" << hp << "->" << (hp - d) << ")\n";
    hp = std::max(0, hp - d);
    if (hp == 0) {
        std::cout << "Warrior killed!\n";
        alive = false;
    }
}

void WarriorNPC::ConsumeAmmo(int k) {
    std::cout << "Warrior ConsumeAmmo: -" << k << " (" << ammo << "->" << (ammo - k) << ")\n";
    ammo = std::max(0, ammo - k);
}

// Mission commands
// NOTE: no default here; default is only in the header!
void WarriorNPC::SetMission(int targetR, int targetC, MissionType type, double rw) {
    std::cout << "Warrior SetMission: type " << type
        << " -> (" << targetR << "," << targetC
        << ") rw=" << rw << "\n";
    missionTarget = { targetR, targetC };
    missionType = type;
    riskWeight = rw;
}

std::pair<int, int> WarriorNPC::GetMissionTarget() const {
    static int getTargetCount = 0;
    if (++getTargetCount % 200 == 0) {
        std::cout << "Warrior GetMissionTarget: ("
            << missionTarget.first << "," << missionTarget.second << ")\n";
    }
    return missionTarget;
}
