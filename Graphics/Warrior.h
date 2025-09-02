// ============== Warrior.h ==============
#ifndef WARRIOR_H
#define WARRIOR_H

#include "Character.h"
#include "Projectile.h"
#include <vector>

enum class CombatState {
    SCANNING,
    TARGETING,
    SHOOTING,
    RELOADING,
    SEEKING_COVER
};

class Warrior : public Character {
private:
    int ammo;
    int grenades;
    int fireRange;
    int grenadeRange;
    CombatState combatState;
    Position lastEnemyPos;
    std::vector<Position> enemiesSpotted;
    int lastShotTime;

public:
    Warrior(Position p, int team);

    void Update(GameMap* map) override;
    void Show() override;

    void ScanForEnemies(std::vector<Character*>& allCharacters);
    Position SelectBestTarget();
    void Shoot(Position target, std::vector<Projectile*>& projectiles);
    void ThrowGrenade(Position target, std::vector<Projectile*>& projectiles);
    void SeekCover(GameMap* map);

    int getAmmo() const { return ammo; }
    int getGrenades() const { return grenades; }
    bool needsAmmo() const { return ammo < 5; }
};

#endif
