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

class Item : public Obj
{
public:
	Item();
	Item& operator=(const Obj& other);
	bool isInRange(const Vec3& playerPos, float radius);

	~Item();

private:
	string name;
	float radius;
	bool inInventory;
};