#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include "Vec3.h"
#include "drawFunc.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <SOIL.h>

class Bullet
{

private:
	float size, velocity;
	Vec3 pos, look;
	chrono::steady_clock::time_point start, end;

public:
	Bullet();
	Bullet(float Size, float Velocity, Vec3 Look, Vec3 Pos, chrono::steady_clock::time_point Start);
	void draw();
	chrono::steady_clock::time_point Start() { return start; };
	chrono::steady_clock::time_point End() { return end; };
};