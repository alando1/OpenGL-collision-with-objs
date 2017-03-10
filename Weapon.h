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
	Weapon();
	Weapon& operator=(const Obj& other);
	void drawWeapon();
	~Weapon();

private:
	string name;

};