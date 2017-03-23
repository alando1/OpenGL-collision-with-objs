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

#include <SDL/SDL.h>
#undef main 	//required by SDL
#include <SDL/SDL_mixer.h>

class Item : public Obj
{
public:
	Item(const char* fileName, const char* Name, Vec3& initialPos);
	Item& operator=(const Obj& other);
	bool isInRange(const Vec3& playerPos, float radius);
	Mix_Chunk* pickupSFX;
	
	Item();
	~Item();

private:
	float radius;
	bool inInventory;
};