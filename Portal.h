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

class Portal
{
private:
	float orientationMatrix[16];
	Vec3 pos, look, right, up;
	chrono::steady_clock::time_point open;
	chrono::steady_clock::time_point close;
	chrono::steady_clock::time_point start;
	chrono::steady_clock::time_point end;
	float increment, increment2, portalAngle;

public:
	Portal(Vec3 CamPos, Vec3 CamLook, chrono::steady_clock::time_point start);
	void openPortal();
	void texturePortal(int testTexture);
	void normalizePortal();
	chrono::steady_clock::time_point Open() { return open; };
	chrono::steady_clock::time_point End() { return end; };
	Vec3 returnPosition(){ return pos; };
};