// Grenade.cpp
#include "Grenade.h"
const double PI = 3.14;

Grenade::Grenade(double xx, double yy)
{
	int i;
	double angle, teta = 2 * PI / NUM_BULLETS;
	x = xx;
	y = yy;
	isExploding = false;
	// create all bullets
	for (i = 0, angle = 0; i < NUM_BULLETS; i++, angle += teta)
		bullets[i] = new Bullet(x, y, angle);
}

void Grenade::Show()
{
	int i;

	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->Show();
}

void Grenade::Explode(int map[MSZ][MSZ])
{
	int i;
	isExploding = true;

	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->SetIsMoving(true);
		bullets[i]->Move(map);
	}


}

void Grenade::CreateScurityMap(int map[MSZ][MSZ], double smap[MSZ][MSZ])
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->SetIsMoving(true);
		bullets[i]->UpdateSecurityMap(map,smap);
	}

}
