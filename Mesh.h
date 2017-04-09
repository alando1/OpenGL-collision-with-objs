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
#include <algorithm>

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <SOIL.h>

using namespace std;

class Mesh
{
public:

	Mesh():
		_verts(NULL), _texCoords(NULL), _normals(NULL), _vertCount(0), _isBuilt(false), _blendingStartChunk(-1)
	{
		//
	}

	Mesh(const char* filePath) :
		_name("mesh"), _verts(NULL), _texCoords(NULL), _normals(NULL), _vertCount(0), _isBuilt(false), _blendingStartChunk(-1)
	{
		loadOBJ(filePath);
	}

	Mesh(const char* filePath, const char* name) :
		_name(name), _verts(NULL), _texCoords(NULL), _normals(NULL), _vertCount(0), _isBuilt(false), _blendingStartChunk(-1)
	{
		loadOBJ(filePath);
	}

	~Mesh()
	{
		//cout << "Mesh::destroy()" << endl;
		destroy();
	}

	void loadOBJ(const char* filePath);
	void render();
	void update(float dt);
	void destroy();

	void enableChunkSpin(string objectName, unsigned int chunkIndex, const glm::vec3& axis, float rotationSpeed);

	void summarize(const char* filePath);

private:

	// representation of a material
	struct Material
	{
		string name;
		glm::vec3 ambientColor;		// Ka
		glm::vec3 diffuseColor;		// Kd
		glm::vec3 specularColor;	// Ks
		float specularExp;			// Ns
		float alpha;				// d
		int illumModel;				// illumination model. model 2 = calculate diffuse, ambient, and specular highlights
		string diffuseMapPath;		// map_Kd
	};

	// represents a chunk of geometry rendered with one material
	struct Chunk
	{
		string materialName;
		float alpha;
		GLuint texture;
		unsigned int startingIndex;
		unsigned int elementCount;
		bool isAnimated;
		glm::vec3 axis;
		float rotationSpeed;
		float rotationAngle;
	};

	// represents a collection of geometry delimited by the 'o' tag.
	// it can contain multiple chunks and thus use multiple materials
	struct Object
	{
		string name;
		list<Chunk> chunks;
	};

	// a reference-counted GL texture.  used so multiple meshes can be
	// instantiated that reference the same texture, but only 1 GL texture is
	// made and shared.
	struct TextureRef
	{
		GLuint texture;
		int refCount;
	};

	// converts the indexed geometry data into non-indexed
	void _buildArrays(vector<glm::vec3>& verts, vector<glm::vec2>& UVs, vector<glm::vec3>& normals, vector<glm::ivec3>& indices);
	// parses the .mtl file for materials and builds the map
	void _loadMaterials(const string& filePath);
	// loads only the textures used in the materials for the chunks that will be rendered
	void _loadTextures();
	// decrements the reference count of this mesh to a particular texture
	void _releaseTexture(GLuint texture);
	// construct a vector of Chunks to use as the ordering for rendering
	void _buildRenderList();
	// yea
	void _printRenderList();

	// it seems that a static class function will suffice as a comparison function for std::sort
	static bool _compareChunkAlphas(Chunk* a, Chunk* b);
	static bool _compareChunkTextures(Chunk* a, Chunk* b);

	string _name;

	// pointers to the actual arrays in memory from which we render
	float* _verts;
	float* _texCoords;
	float* _normals;
	int _vertCount;

	// more useful stuff
	bool _isBuilt;
	// the filepath of the .obj file loaded
	string _sourceFilePath;
	// the index into the render list to start rendering blended chunks
	int _blendingStartChunk;
	
	// map between a material name and the material properties
	map<string, Material> _materials;
	// list of objects, each containing one or more chunks
	list<Object> _objects;
	// map between texture map file paths to the actual GL texture name.
	// the map is static to the whole class, and the textures are reference counted
	static map<string, TextureRef> _textureFilePathMap;
	// vector of chunk pointers specifying the render order of said chunks
	vector<Chunk*> _renderList;
	// vector of chunks that have some kind of animation enabled, and thus need
	// to be checked each frame with update()
	vector<Chunk*> _animateList;
};
