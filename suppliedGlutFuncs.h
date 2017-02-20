#pragma once
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <chrono>
#include "Vec3.h"
#include "Portal.h"
#include "drawFunc.h"
#include "Bullet.h"

#include <list>
#include <stdio.h>
#include <SDL.h>
#include <SDL_mixer.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

extern void renderScene(void);

void initGL(int argc, char** argv);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);
void pressNormalKeys(unsigned char key, int xx, int yy);
void releaseNormalKeys(unsigned char key, int xx, int yy);
void pressSpecialKeys(int key, int xx, int yy);
void releaseSpecialKeys(int key, int x, int y);
void renderBitmapString(float d, float e, float f, void *font, char *string);
void renderText2D(float a, float b, void* font, char* string);
void printScreenText();
void changeSize(int w, int h); 
