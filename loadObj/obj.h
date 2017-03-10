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

class Obj
{
public:
	const char* objName;
	GLfloat* vertices;
	GLfloat* textures;
	GLfloat* normals;
	GLint* indices;

	int numOfFaces;
	vector<Vec3> tmpVecArray;
	vector<Vec2> tmpUVsArray;
	vector<Vec3> tmpNrmArray;
	vector<int> tmpIndxArray;

	Obj(const char* fileName)
	{
		objName = fileName;
		ifstream pFile;
		pFile.open(fileName);
		string token;
		string line;
		bool ignoreTexCoords;

		//int read;
		char ch;
		float u, v, x, y, z;
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
					//tmpIndxArray.push_back(b);

					
					//cout << " " << a << "/" << b << "/" << c;
				}
				numOfFaces++;
				//cout << endl;
			}

			//takes next available token
			pFile >> token;		
		}

		int p = 0;
		vertices = new GLfloat[tmpVecArray.size() * 3];
		for (auto i = tmpVecArray.begin(); i != tmpVecArray.end(); ++i)
		{
			vertices[p++] = i->x;
			vertices[p++] = i->y;
			vertices[p++] = i->z;
		}

		p = 0;
		textures = new GLfloat[tmpUVsArray.size() * 2];
		for (auto i = tmpUVsArray.begin(); i != tmpUVsArray.end(); ++i)
		{
			textures[p++] = i->x;
			textures[p++] = i->y;
		}

		p = 0;
		normals = new GLfloat[tmpNrmArray.size() * 3];
		for (auto i = tmpNrmArray.begin(); i != tmpNrmArray.end(); ++i)
		{
			normals[p++] = i->x;
			normals[p++] = i->y;
			normals[p++] = i->z;
		}

		p = 0;
		indices = new GLint[tmpIndxArray.size()];
		for (auto i = tmpIndxArray.begin(); i != tmpIndxArray.end(); ++i)
		{
			indices[p++] = *i;
		}

	    glVertexPointer(3, GL_FLOAT, 0, vertices);
	    glTexCoordPointer(2, GL_FLOAT, 0, textures);
	    glNormalPointer(GL_FLOAT, 0, normals);
	    //glColorPointer(3, GL_FLOAT, 0, colsPointer);
	}
	void draw()
	{
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	    glEnableClientState(GL_NORMAL_ARRAY);
	    //glEnableClientState(GL_COLOR_ARRAY);
	    glColor4f(1,1,1,1);
/*	    glPolygonMode(GL_FRONT, GL_LINE);
	    glPolygonMode(GL_BACK, GL_LINE);*/

		glDrawElements(GL_TRIANGLES, numOfFaces*3, GL_UNSIGNED_INT, indices);

	    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	    glDisableClientState(GL_NORMAL_ARRAY);
	    //glDisableClientState(GL_COLOR_ARRAY);
	}

	~Obj()
	{
		delete [] vertices;
		delete [] textures;
		delete [] normals;
		delete [] indices;
	}
};