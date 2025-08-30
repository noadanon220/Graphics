#include "Grenade.h"
#include <iostream> // for logging
const double PI = 3.14;

Grenade::Grenade(double xx, double yy)
{
    x = xx; y = yy;
    isExploding = false;
    std::cout << "GRENADE: created at (" << (int)x << "," << (int)y << ") with " << NUM_BULLETS << " bullets" << std::endl;
    double teta = 2 * PI / NUM_BULLETS;
    for (int i = 0; i < NUM_BULLETS; ++i) {
        double angle = i * teta;
        bullets[i] = new Bullet(x, y, angle);
    }
}

void Grenade::Show() {
    static int showCount = 0;
    if (++showCount % 500 == 0) { // Log every 500 renders
        std::cout << "GRENADE: rendering at (" << (int)x << "," << (int)y << "), exploding: " << (isExploding ? "YES" : "NO") << std::endl;
    }
    
    for (int i = 0; i < NUM_BULLETS; ++i)
        bullets[i]->Show();
}

void Grenade::Explode(int map[MSZ][MSZ]) {
    isExploding = true;
    std::cout << "GRENADE: exploding at (" << (int)x << "," << (int)y << ") with " << NUM_BULLETS << " bullets" << std::endl;
    for (int i = 0; i < NUM_BULLETS; ++i) {
        bullets[i]->SetIsMoving(true);
        bullets[i]->Move(map);
    }
}

void Grenade::CreateScurityMap(int map[MSZ][MSZ], double smap[MSZ][MSZ]) {
    std::cout << "GRENADE: updating security map at (" << (int)x << "," << (int)y << ")" << std::endl;
    for (int i = 0; i < NUM_BULLETS; ++i) {
        bullets[i]->SetIsMoving(true);
        bullets[i]->UpdateSecurityMap(map, smap);
    }
}
