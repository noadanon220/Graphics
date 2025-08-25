// Bullet.h
#pragma once
#include "Definitions.h"


class Bullet
{

private:
	double x, y;   // location in map
	double dirx, diry; // direction of bullet
	bool isMoving;

public:
	Bullet();
	Bullet(double xx, double yy, double alpha);
	void Show();
	void Move(int map[MSZ][MSZ]);
	void SetIsMoving(bool value) { isMoving = value; }
	void UpdateSecurityMap(int map[MSZ][MSZ], double smap[MSZ][MSZ]);
};


