#pragma once
#include <cstdlib>
#include "string.h"
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

	Vec3 look, right, up;
	float orientationMatrix[16];

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
		look = Vec3(0,0,-1);
		right = Vec3(1,0,0);
		up = Vec3(0,1,0);
		updateMatrix();
		scale.x = 1; scale.y = 1; scale.z = 1;
		rotAxes.x = 0; rotAxes.y = 0; rotAxes.z = 0;
		collisionVec3.x = 0; collisionVec3.y = 0; collisionVec3.z = 0;

		buildObj();
	}

	void buildObj()
	{
		mesh = new Mesh(fileName.c_str(), name.c_str());
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


	}

	void lookAt(const Vec3& p)
	{
		Vec3 tmpLook = p - pos;
		tmpLook.normalize();
		look = tmpLook;

		Vec3 tmpUp(0,1,0);
		right = look.cross(tmpUp);
		up = right.cross(look);

		right.normalize();
		up.normalize();
		updateMatrix();
		

	}

	void draw()
	{
		glPushMatrix();
/*		if(name == "portalGun")
			drawPortalGun(dt);
		else
		{*/
			glTranslatef(pos.x,pos.y,pos.z);
			glMultMatrixf(orientationMatrix);
			if(rotAxes.x == 0 && rotAxes.y == 0 && rotAxes.z == 0 && rot == 0)
			{
				//cout << name << " object has rotAxes (0,0,0)" << endl;
			}
			else
				glRotatef(rot, rotAxes.x, rotAxes.y, rotAxes.z);
			
			glScalef(scale.x,scale.y,scale.z);
/*			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);*/

			mesh->render();
/*			glDisable(GL_CULL_FACE);*/
		//}
		glPopMatrix();
	}

	void updateMatrix()
	{
		memset(orientationMatrix, 0, sizeof(float)*16);
		
		orientationMatrix[0]=right.x;
		orientationMatrix[1]=right.y;
		orientationMatrix[2]=right.z;
		orientationMatrix[3]=0;

		orientationMatrix[4]=up.x;
		orientationMatrix[5]=up.y;
		orientationMatrix[6]=up.z;
		orientationMatrix[7]=0;

		orientationMatrix[8]=-look.x;
		orientationMatrix[9]=-look.y;
		orientationMatrix[10]=-look.z;
		orientationMatrix[11]=0;

		orientationMatrix[12]=0;
		orientationMatrix[13]=0;
		orientationMatrix[14]=0;
		orientationMatrix[15]=1.0f;
	}
};