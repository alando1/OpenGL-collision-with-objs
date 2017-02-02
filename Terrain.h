#pragma once
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Vec3.h"

class Terrain
{
	private:
		int nSize;
		float midPoint;
		int length;
		int width;
		float scalar;
		float* heights;
		int tileSize;
		float heightRatio;
		float* vertices;

		void buildHeightMap(unsigned char* buffer);

	public:
		Terrain(float len, float wid);
		~Terrain();
		void loadFile(const char* fileName, int nSize);
		void draw(bool wireFrame);
		void setVertexColor(int x, int z);
		float* returnHeights();
		float scale();
		void draw2(bool wireFrame);
		void setVertexColor2(int x, int z);
		Vec3 returnNormal(Vec3& x,Vec3& y,Vec3& z);
		void normalizeTerrain();
};
