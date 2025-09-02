#include "Bullet.h"
#include <math.h>
#include "glut.h"
#include <iostream> // for logging

Bullet::Bullet() {
    static int bulletDefaultCount = 0;
    if (++bulletDefaultCount % 100 == 0) { // Log every 100 default bullets
        std::cout << "BULLET: default constructor called" << std::endl;
    }
}
Bullet::Bullet(double xx, double yy, double alpha)
{
    x = xx; y = yy;
    isMoving = false;
    dirx = cos(alpha);
    diry = sin(alpha);
    static int bulletCount = 0;
    if (++bulletCount % 100 == 0) { // Log every 100 bullets
        std::cout << "BULLET: created at (" << (int)x << "," << (int)y << ") with direction (" << dirx << "," << diry << ")" << std::endl;
    }
}

void Bullet::Show()
{
    static int showCount = 0;
    if (++showCount % 1000 == 0) { // Log every 1000 renders
        std::cout << "BULLET: rendering at (" << x << "," << y << "), moving: " << (isMoving ? "YES" : "NO") << std::endl;
    }
    
    glColor3d(1, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2d(x, y + 0.5);
    glVertex2d(x - 0.5, y);
    glVertex2d(x, y - 0.5);
    glVertex2d(x + 0.5, y);
    glEnd();
}

void Bullet::Move(int map[MSZ][MSZ])
{
    double tmpx = x + SPEED * dirx;
    double tmpy = y + SPEED * diry;
    if (tmpx < 0 || tmpx >= MSZ || tmpy < 0 || tmpy >= MSZ) {
        static int outOfBoundsCount = 0;
        if (++outOfBoundsCount % 100 == 0) { // Log every 100 out-of-bounds
            std::cout << "BULLET: out of bounds at (" << tmpx << "," << tmpy << "), stopping" << std::endl;
        }
        isMoving = false;
    }
    else {
        int cell = map[(int)tmpy][(int)tmpx];
        if (cell == STONE || cell == TREE) {
            static int blockedCount = 0;
            if (++blockedCount % 100 == 0) { // Log every 100 blocks
                std::cout << "BULLET: blocked at (" << tmpx << "," << tmpy << ") by terrain type " << cell << std::endl;
            }
            isMoving = false;
        }
    }
    if (isMoving) {
        x = tmpx;
        y = tmpy;
    }
}

void Bullet::UpdateSecurityMap(int map[MSZ][MSZ], double smap[MSZ][MSZ])
{
    int steps = 0;
    while (isMoving && steps < 100) { // Prevent infinite loops
        smap[(int)y][(int)x] += SECURITY_VALUE;
        Move(map);
        steps++;
    }
    if (steps > 0) {
        std::cout << "BULLET: updated security map for " << steps << " steps, final pos (" 
                  << (int)x << "," << (int)y << ")" << std::endl;
    }
}
