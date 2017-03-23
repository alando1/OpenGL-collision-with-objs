#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <vector>
#include "Vec3.h"
#include "Mesh.h"
#include "DrawFunc.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "SOIL.h"
#include <glm/glm.hpp>


using namespace std;
extern float dt;

class Obj
{
public:

	string fileName;
	string name;
	string type;
	float rot;
	int texture;
	bool collision;
	int numOfFaces;
	float portAngle;
	Vec3 pos;
	Vec3 scale;
	Vec3 rotAxes;
	Vec3 collisionVec3;
	Mesh* mesh;

	Vec3 returnPos() { return pos; }; 

	Obj()
	{
		name = "portalGun";
	}

	Obj(const char* _fileName, const char* Name)
	{
		name = Name;
		fileName = string(_fileName);
		rot = 0;
		portAngle = 0;
		numOfFaces = 0;
		collision = false;
		pos.x = 0; pos.y = 0; pos.z = 0;
		scale.x = 0; scale.y = 0; scale.z = 0;
		rotAxes.x = 0; rotAxes.y = 0; rotAxes.z = 0;
		collisionVec3.x = 0; collisionVec3.y = 0; collisionVec3.z = 0;

		buildObj();
	}

	void buildObj()
	{
		mesh = new Mesh(fileName.c_str(), name.c_str());

	}

	void draw()
	{
		glPushMatrix();
			adjust();

			if(name == "portalGun")
				drawPortalGun(dt);
			else	
				mesh->render();
		
/*				cout << "object drawn at:" << endl;
				cout << this->pos.x << "," << this->pos.y << "," << this->pos.z << endl;
				cout << "scale : " << scale.x << endl;*/
		glPopMatrix();
	}
	~Obj()
	{
	}

	void setPos(float x, float y, float z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}

	void setPos(Vec3& newPos)
	{
		pos = newPos;
	}

	void setRot(float angle, float x, float y, float z)
	{
		rot = angle;
		rotAxes.x = x;
		rotAxes.y = y;
		rotAxes.z = z; 

	}

	void setScale(float scalar)
	{
		scale.x = scalar;
		scale.y = scalar;
		scale.z = scalar;
	}

	void setScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	string getName()
	{
		return name;
	}

	void adjust()
	{
		if(pos.x == 0 && pos.y == 0 && pos.z == 0)
		{
			//cout << name << " object has pos (0,0,0)" << endl;
		}
		else
			glTranslatef(pos.x, pos.y, pos.z);

		if(rotAxes.x == 0 && rotAxes.y == 0 && rotAxes.z == 0 && rot == 0)
		{
			//cout << name << " object has rotAxes (0,0,0)" << endl;
		}
		else
			glRotatef(rot, rotAxes.x, rotAxes.y, rotAxes.z);

		if(scale.x == 0 && scale.y == 0 && scale.z == 0)
		{
			//cout << name << " object has scale (0,0,0)" << endl;
		}
		else
			glScalef(scale.x, scale.y, scale.z);	
	}
};