#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include "Vec3.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <SOIL.h>

void drawgrid();
void otherGrid();
void drawFloor();
void texturePlane(int testTexture, float length, float width);
void buildHouse();
void drawShed();
void drawPortalGun(float dt);
void weaponRecoil();
void drawTest(float dt);

//void setVSync(int i);
