// ============== Game.h ==============
#ifndef GAME_H
#define GAME_H

#include "GameMap.h"
#include "Character.h"
#include "Commander.h"
#include "Warrior.h"
#include "Medic.h"
#include "Supplier.h"
#include "Projectile.h"
#include <vector>

class Game {
private:
    GameMap* map;
    std::vector<Character*> team1;
    std::vector<Character*> team2;
    std::vector<Projectile*> activeProjectiles;
    Commander* commander1;
    Commander* commander2;
    bool gameRunning;
    int gameTime;

public:
    Game();
    ~Game();

    void Initialize();
    void Update();
    void Show();
    void CheckCollisions();
    bool CheckGameEnd();

    void ShowGameStats();
    void ShowVisibilityMap(int team);

    GameMap* getMap() { return map; }
    std::vector<Character*> getAllCharacters();
    std::vector<Character*> getEnemyTeam(int team);
};

#endif