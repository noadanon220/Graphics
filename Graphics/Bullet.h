#pragma once
#include "Definitions.h"

// Simple bullet used for grenades (risk accumulation).
class Bullet {
private:
    double x, y;
    double dirx, diry;
    bool isMoving;
public:
    Bullet();
    Bullet(double xx, double yy, double alpha);
    void Show();
    void Move(int map[MSZ][MSZ]);
    void SetIsMoving(bool v) { isMoving = v; }
    void UpdateSecurityMap(int map[MSZ][MSZ], double smap[MSZ][MSZ]);
};
