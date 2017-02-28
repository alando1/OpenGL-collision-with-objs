#pragma once
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "Vec3.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

struct color4
{
	float r, g, b, a;
};

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


		GLfloat* vertices;
		GLfloat* textures;
		GLfloat* normals;
		GLfloat* colors;
		GLint* indices;

		int numOfVerts;
		int colorIncrement;
		vector<Vec3> tmpVecArray;
		vector<Vec2> tmpUVsArray;
		vector<Vec3> tmpNrmArray;
		vector<color4> tmpColArray;
		vector<int> tmpIndxArray;


		void buildHeightMap(unsigned char* buffer);

	public:
		Terrain(float len, float wid);
		~Terrain();
		void loadFile(const char* fileName, int nSize);
		void draw();
		void draw2();
		float scale();
		void drawElements();
		float* returnHeights();
		void normalizeTerrain();
		color4 setColorArray(float y);
		void setVertexColor(int x, int z);
		void setVertexColor2(int x, int z);
		Vec3 returnNormal(Vec3& x,Vec3& y,Vec3& z);
};