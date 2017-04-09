#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#include <chrono>
#include "Vec3.h"

class Camera
{

	private:
		Vec3 pos, look, up, right;

	public:
		Camera(Vec3& pos,Vec3& look,Vec3& up);
		Camera();
		~Camera();

		void setView();
		void setPos(Vec3& p){	pos = p;	};
		Vec3& getPos(){	return pos; };
		void setLook(Vec3& other){	look = other;	};
		Vec3& getLook(){ return look;	};

	
};