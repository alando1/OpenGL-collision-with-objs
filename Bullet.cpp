#include "Bullet.h"

	Bullet::Bullet(float Size, float Velocity, Vec3 Look, Vec3 Pos, chrono::steady_clock::time_point Start) :
	size(Size), velocity(Velocity), look(Look), pos(Pos), start(Start)
	{

		color.r=0.1f;
		color.g=0.1f;
		color.b=0.1f;
		pos.x+=look.x*5;
		pos.y+=look.y*5;
		pos.z+=look.z*5;
		chrono::milliseconds tmp = chrono::milliseconds(20000);
		end = start + tmp;

	}

	void Bullet::draw()
	{
		auto now = chrono::steady_clock::now();

		float dt = chrono::duration_cast<chrono::duration<float, milli>>(now-start).count();
		dt = dt/1000.0f;
		
		pos.x+=look.x*50*dt;
		pos.y+=look.y*50*dt;
		pos.z+=look.z*50*dt;
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		//glColor3f(color.r,color.g,color.b);
		glutSolidSphere(size, 10, 10);
		glPopMatrix();
		//cout << "bullet drawn" << endl;
	}
