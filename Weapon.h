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
#include "Item.h"

class Weapon : public Item
{
public:
	Weapon(const char* fileName, const char* Name, Vec3& initialPos, Vec3& finalPos, float initialScale, float finalScale,
				float w, float x, float y, float z, const char* shootTexture);
	Weapon(const char* name, Vec3& initPos);
	Weapon& operator=(const Obj& other);
	void reposition();
	void drawFire();
	void draw();
	~Weapon();
	string type;
	void setShootTrue(){ isFiring = true; };

private:
	int fire;
	bool isFiring;
	Vec3 initial, aim, finalPos;
	float w,x,y,z,finalScale;
	int firingCount;

};