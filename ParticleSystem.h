#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <SOIL.h>

using namespace std;

#ifndef PI
	#define PI 3.14159265f
#endif

#ifndef PI_OVER_180
	#define PI_OVER_180	(PI / 180.0f)
#endif

// types of particle systems
#define PST_BLOOM	1


class ParticleSystem
{
public:

	/*
	ParticleSystem():
		_type(0), _particles(NULL), _particleCount(0), _particleTexture(0),
		_verts(NULL), _texCoords(NULL), _vertCount(0)
	{
		//
	}*/

	ParticleSystem(int type, int particleCount, string& texturePath):
		_origin(0.0f, 0.0f, 0.0f), _type(type), _particleCount(particleCount)
	{
		_build(texturePath);
		// init
		update(0.0f);
	}
	
	~ParticleSystem()
	{
		destroy();
	}

	void update(float dt);
	void reset();
	void render(const glm::vec3& camPos);
	void destroy();

private:

	struct Particle
	{
		glm::vec3 pos;
		glm::vec3 vel;
		float lifeTime;
		float alpha;
	};

	void _build(string& texturePath);
	//void _updateQuad(Particle* p);
	void _updateBloom(float dt);
	void _initParticleBloom(Particle* p);

	glm::vec3 _origin;

	int _type;
	Particle* _particles;
	int _particleCount;
	GLuint _particleTexture;

	float* _verts;
	float* _texCoords;
	int _vertCount;
};
