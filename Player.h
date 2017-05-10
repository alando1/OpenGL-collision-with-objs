#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <vector>
#include "Vec3.h"
#include "Obj.h"
#include "Terrain.h"
#include <SDL.h>
//#undef main 	//required by SDL
#include <SDL_mixer.h>

class Player : public Obj
{
public:
	Player(const char* fileName, const char* Name, Vec3& initialPos, float eyeHeight, Vec3& looks);
	Player();
	~Player();
	
	int deaths;
	Player* target;
	Terrain* terrain;

	float eye();
	void updateCamera();
	void persue(float dt);
	void attack(Player* _target);
	void setTarget(Player* _target);
	void setTerrain(Terrain* t)
	{
		terrain = t;
	};
	void setGrunt(const char* fileLoc);
	void setBodyCenter(float bodyCenter);
	float getHealth()		{		return health;	};
	void setLook(Vec3& v)	{		look = v;		};
	void setDead()			{alive = false;++deaths;};
	void setAlive();
	bool isAlive()			{		return alive;	};
	bool isDeadCam()		{		return deadCam;	};
	void setDeadCam(bool tf){		deadCam = tf;	};

	Vec3 returnCollisionVec();

private:
	float eyeHeight, bodyCenter, health, speed;
	bool alive, readyToAttack, deadCam;

	chrono::steady_clock::time_point nextAttackTime, lastAttackTime;
	Mix_Chunk* attackGrunt;

};
