#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include "Vec3.h"
#include "DrawFunc.h"
#include "Terrain.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "SOIL.h"

/*struct color4
{
	float r, g, b, a;
};*/

class Bullet
{

private:
	float size, velocity;
	Vec3 look, pos;
	color4 color;
	chrono::steady_clock::time_point start, end;

public:
	Bullet();
	Bullet(float Size, float Velocity, Vec3 Look, Vec3 Pos, chrono::steady_clock::time_point Start);
	void draw();
	Vec3 returnPos() { return pos; };
	Vec3 returnLook() { return look; };
	chrono::steady_clock::time_point Start() { return start; };
	chrono::steady_clock::time_point End() { return end; };
};