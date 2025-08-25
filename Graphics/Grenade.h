// Grenade.h
#pragma once
#include "Bullet.h"

const int NUM_BULLETS = 20;

class Grenade
{
private:
	Bullet* bullets[NUM_BULLETS];
	double x, y;
	bool isExploding;

public:
	Grenade(double xx, double yy);
	void Show();
	void Explode(int map[MSZ][MSZ]);
	void CreateScurityMap(int map[MSZ][MSZ], double smap[MSZ][MSZ]);
};

