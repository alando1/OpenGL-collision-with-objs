#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <vector>
#include "Vec3.h"

class Mesh
{

private:

	string meshName;
	string meshText;


public:

	Mesh();
	string getName() {	return meshName; };
	string getText() {	return meshText; };

};