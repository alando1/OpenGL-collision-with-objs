/*
	OpenGL 2.1'ish mesh class rendered with vertex arrays and loaded from Wavefront .obj's.

	Mar 2017
*/

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include "Vec3.h"
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "SOIL.h"

using namespace std;

class Mesh
{
public:
	Mesh():
		_verts(NULL), _texCoords(NULL), _normals(NULL), _vertCount(0)
	{
		//
	}

	Mesh(const char* filePath, const char* Name) :
		_verts(NULL), _texCoords(NULL), _normals(NULL), _vertCount(0)
	{
		loadOBJ(filePath, Name);
	}

	~Mesh()
	{
		destroy();
	}

	void loadOBJ(const char* filePath, const char* Name);
	void render();
	void destroy();

private:
	// crude representation of a material; really only care about the diffuse texture for now
	struct Material
	{
		string name;
		string diffuseMapPath;
	};

	// represents a collection of elements rendered using a certain texture, tracked by the
	// usemtl tokens in the .OBJ file
	struct Chunk
	{
		string materialName;
		GLuint texture;
		unsigned int startingIndex;
		unsigned int elementCount;
	};

	void _buildArrays(vector<glm::vec3>& verts, vector<glm::vec2>& UVs, vector<glm::vec3>& normals, vector<glm::ivec3>& indices);
	void _loadMaterials(const string& filePath);
	void _loadTextures();



	float* _verts;
	float* _texCoords;
	float* _normals;
	int _vertCount;

	// map between a material name and the material properties
	map<string, Material> _materials;
	// a list of chunks that each render with their own texture bound
	list<Chunk> _chunks;
};
