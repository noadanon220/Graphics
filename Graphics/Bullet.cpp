// Bullet.cpp
#include "Bullet.h"
#include <math.h>
#include "glut.h"

Bullet::Bullet()
{
}

Bullet::Bullet(double xx, double yy, double alpha)
{
	x = xx;
	y = yy;
	isMoving = false;
	dirx = cos(alpha);
	diry = sin(alpha);
}

void Bullet::Show()
{
	glColor3d(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x, y + 0.5);
	glVertex2d(x-0.5, y );
	glVertex2d(x, y- 0.5);
	glVertex2d(x+ 0.5, y );
	glEnd();
}

void Bullet::Move(int map[MSZ][MSZ])
{
	double tmpx = x + SPEED * dirx;
	double tmpy = y + SPEED * diry;

	// stop if outside bounds
	if (tmpx < 0 || tmpx >= MSZ || tmpy < 0 || tmpy >= MSZ) {
		isMoving = false;
	}
	else {
		int cell = map[(int)tmpy][(int)tmpx];
		// stop only if hit stone or tree
		if (cell == STONE || cell == TREE) {
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
	while (isMoving)
	{
		smap[(int)y][(int)x] += SECURITY_VALUE;
		Move(map);
	}
}
