#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <vector>
#include "Vec3.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <SOIL.h>

using namespace std;
extern float dt;

class Obj
{
public:
	string name;
	string objName;
	const char* textureLocation;
	GLfloat* vertices;
	GLfloat* textures;
	GLfloat* normals;
	GLint* indices;

	vector<Vec3> tmpVecArray;
	vector<Vec2> tmpUVsArray;
	vector<Vec3> tmpNrmArray;
	vector<int> tmpIndxArray;

	float rot;
	int texture;
	bool collision;
	int numOfFaces;
	float portAngle;
	Vec3 pos;
	Vec3 scale;
	Vec3 rotAxes;
	//Vec3 translate;
	Vec3 collisionVec3;

	Vec3 returnPos() { return pos; }; 

	Obj()
	{
		vertices = NULL;
		textures = NULL;
		normals = NULL;
		indices = NULL;
		name = "portalGun";
	//	cout << "? Base class constructor "<< endl;
	}

	Obj(const char* fileName, const char* locationOfTexture)
	{
		textureLocation = locationOfTexture;
		name = fileName;

		rot = 0;
		portAngle = 0;
		numOfFaces = 0;
		collision = FALSE;
		pos.x = 0; pos.y = 0; pos.z = 0;
		scale.x = 0; scale.y = 0; scale.z = 0;
		rotAxes.x = 0; rotAxes.y = 0; rotAxes.z = 0;
		//translate.x = 0; translate.y = 0; translate.z = 0;
		collisionVec3.x = 0; collisionVec3.y = 0; collisionVec3.z = 0;

		buildObj();
	}

	void buildObj()
	{
		if(textureLocation != NULL)
			texture = SOIL_load_OGL_texture(textureLocation, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

		ifstream pFile;
		pFile.open(name);
		string token;
		string line;
		string groupName;
		char ch;
		float s, u, v, x, y, z;
		int a, b, c;

		if(pFile.fail())
		{
			cout << "> file pointer failed, exiting" << endl;
			return;
		}

		//takes next available token
		pFile >> token;

		while( !(pFile.eof() || pFile.fail()) )
		{
			if(token == "#")
			{
				getline(pFile, line);
			}
			else if (token == "o")
			{
				pFile >> objName;
			}
			else if (token == "g")
			{
				getline(pFile, groupName);
			}
			else if(token == "s")
			{
				getline(pFile, line);
				//pFile >> s;
			}
			else if(token == "mtllib")
			{
				getline(pFile, line);
			}
			else if(token == "usemtl")
			{
				getline(pFile, line);
			}
			else if(token == "v")
			{
				pFile >> x >> y >> z;
				tmpVecArray.push_back(Vec3(x,y,z));

			}
			else if(token == "vt")
			{
				pFile >> u >> v;
				tmpUVsArray.push_back(Vec2(u,v));

			}
			else if(token == "vn")
			{
				pFile >> x >> y >> z;
				tmpNrmArray.push_back(Vec3(x,y,z));
			}
			else if(token == "f")
			{	//assusmes triangle faces (3 sides)

				//cout << "f";
				for(int i=0; i<3; i++)
				{
					
					pFile >> a >> ch >> b >> ch >> c;
					tmpIndxArray.push_back(a-1);
					
					//cout << " " << a << "/" << b << "/" << c;
				}
				numOfFaces++;
				//cout << endl;
			}
			else
				getline(pFile, line);


			//takes next available token
			pFile >> token;		
		}

		int p = 0;
		if (tmpVecArray.size() > 0)
		{
			vertices = new GLfloat[tmpVecArray.size() * 3];
			for (auto i = tmpVecArray.begin(); i != tmpVecArray.end(); ++i)
			{
				vertices[p++] = i->x;
				vertices[p++] = i->y;
				vertices[p++] = i->z;
			}
		}

		p = 0;
		if (tmpUVsArray.size() > 0)
		{
			textures = new GLfloat[tmpUVsArray.size() * 2];
			for (auto i = tmpUVsArray.begin(); i != tmpUVsArray.end(); ++i)
			{
				textures[p++] = i->x;
				textures[p++] = i->y;
			}
		}

		p = 0;
		if (tmpNrmArray.size() > 0)
		{
			normals = new GLfloat[tmpNrmArray.size() * 3];
			for (auto i = tmpNrmArray.begin(); i != tmpNrmArray.end(); ++i)
			{
				normals[p++] = i->x;
				normals[p++] = i->y;
				normals[p++] = i->z;
			}
		}

		p = 0;
		if (tmpIndxArray.size() > 0)
		{
			indices = new (nothrow) GLint[tmpIndxArray.size()];

			if (indices != NULL)
			{
				for (auto i = tmpIndxArray.begin(); i != tmpIndxArray.end(); ++i)
				{
					indices[p++] = *i;
				}
			}
			else 
				cout << name << " indices failed to allocate" << endl;
		}

		pFile.close();
	}

	void drawPortalGun(float dt)
	{
		GLUquadricObj* quadric;
		quadric = gluNewQuadric();

		glPushMatrix();
		glScalef(0.008f,0.008f,0.008f);
		glBegin(GL_QUADS);
			glColor3f(0.8f,0.8f,0.8f);
			glVertex3f(0,0,0);
			glVertex3f(0,5,0);
			glVertex3f(0,5,20);
			glVertex3f(0,0,20);

			glNormal3f(-1,0,0);

			glVertex3f(10,0,0);
			glVertex3f(10,5,0);
			glVertex3f(10,5,20);
			glVertex3f(10,0,20);

			glNormal3f(1,0,0);

			glVertex3f(0,5,0);
			glVertex3f(10,5,0);
			glVertex3f(10,5,20);
			glVertex3f(0,5,20);

			glNormal3f(0,1,0);

			glVertex3f(0,0,0);
			glVertex3f(10,0,0);
			glVertex3f(10,0,20);
			glVertex3f(0,0,20);

			glNormal3f(0,-1,0);
		glEnd();

		glRotatef(90, 0,1,0);
		glTranslatef(0,2.5,0);
		gluCylinder(quadric, 2.5, 2.5, 10, 15, 15);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0,0.7f,0,0.5f);
		glTranslatef(-10,5, 5);

		glPushMatrix();

			portAngle += (360.0f * dt / 1000.0f);
			glRotatef(portAngle, 0.5f, 0.5f, 0.5f);
			gluSphere(quadric, 2, 5, 5);
			//cout << "portAngle: " << portAngle << endl;
		glPopMatrix();

		glRotatef(90,1,0,0);
		glTranslatef(0,0,-5);
		glColor4f(1,1,1,0.25f);
		gluCylinder(quadric, 2.5, 2.5, 9, 15, 15);

		glColor3f(0.8f,0.8f,0.8f);
		glRotatef(-45,0,-1,0);
		glTranslatef(0,0,15);
		//glTranslatef(-10,0,10);
		gluCylinder(quadric, 2.5, 2.5, 9, 15, 15);

		glDisable(GL_BLEND);
		glPopMatrix();
	}

	void draw()
	{
		if(name == "portalGun")
		{
			drawPortalGun(dt);
		}
		else
		{
		    //glEnableClientState(GL_COLOR_ARRAY);
			// glColor4f(1,1,1,1);
		    glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, vertices);

			if (textures != NULL)
			{
				//cout << name << "\ttexture != NULL" << endl;
				glBindTexture(GL_TEXTURE_2D, texture);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, textures);
			}
			else
			{
				//cout << name << "\t.texture == NULL" << endl;
			}

			if (normals != NULL)
			{
				//cout << name << "normals != NULL" << endl;
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT, 0, normals);
			}
			else
			{
				//cout << name << "\t.normals == NULL" << endl;
			}

			if(pos.x == 0 && pos.y == 0 && pos.z == 0)
			{
				//cout << name << " object has pos (0,0,0)" << endl;
			}
			else
			{
				glTranslatef(pos.x, pos.y, pos.z);
			}

			if(rotAxes.x == 0 && rotAxes.y == 0 && rotAxes.z == 0 && rot == 0)
			{
				//cout << name << " object has rotAxes (0,0,0)" << endl;
			}
			else
			{
				glRotatef(rot, rotAxes.x, rotAxes.y, rotAxes.z);
			}

			if(scale.x == 0 && scale.y == 0 && scale.z == 0)
			{
				//cout << name << " object has scale (0,0,0)" << endl;
			}
			else
				glScalef(scale.x, scale.y, scale.z);


		    if (indices != NULL)
		    {

		    	glDrawElements(GL_TRIANGLES, numOfFaces*3, GL_UNSIGNED_INT, indices);

		    }
		    else
		    	cout << "failed to draw elements, " << name << ", indices is null";
		    
		    glDisableClientState(GL_VERTEX_ARRAY);  			  // disable vertex array
		    glDisableClientState(GL_TEXTURE_COORD_ARRAY);		 // disable texture array
		    glDisableClientState(GL_NORMAL_ARRAY);				// disable normal array
		    //glDisableClientState(GL_COLOR_ARRAY);
		}
	}
	~Obj()
	{
		delete [] vertices;
		delete [] textures;
		delete [] normals;
		delete [] indices;
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
/*	void setTranslate(float x, float y, float z)
	{
		translate.x = x;
		translate.y = y;
		translate.z = z;
	}*/

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

/*	Obj& operator=(const Obj& other)
	{
		this->name = other.name;
		this->objName = other.objName;
		this->vertices = other.vertices;
		this->textures = other.textures;
		this->normals = other.normals;
		this->indices = other.indices;

		this->portAngle = other.portAngle;
		this->collision = other.collision;
		this->texture = other.texture;
		this->numOfFaces = other.numOfFaces;
		this->pos = other.pos;
		this->collisionVec3 = other.collisionVec3;
		this->tmpVecArray = other.tmpVecArray;
		this->tmpUVsArray = other.tmpUVsArray;
		this->tmpNrmArray = other.tmpNrmArray;
		this->tmpIndxArray = other.tmpIndxArray;

		return *this;
	}*/
};