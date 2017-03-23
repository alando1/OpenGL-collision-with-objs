#include "Weapon.h"

Weapon::Weapon(const char* fileName, const char* Name, Vec3& initialPos, Vec3& final_pos, float initialScale, float final_scale,
				float W, float X, float Y, float Z, const char* shootTexture) :
	Item(fileName, Name, initialPos), finalPos(final_pos), finalScale(final_scale), w(W), x(X), y(Y), z(Z), firingCount(25)
{
//	cout << "Weapon::Weapon(...), setting type to Weapon" << endl;
	type = "Weapon";
	this->pos = initialPos;
	this->scale.x = initialScale;
	this->scale.y = initialScale;
	this->scale.z = initialScale;

	isFiring = false;

	if(shootTexture != NULL)
		fire = SOIL_load_OGL_texture( shootTexture, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

}

Weapon::Weapon(const char* name, Vec3& initPos) :
	Item("", name, initPos)
{
//	cout << "Weapon::Weapon(...)2, setting type to Weapon" << endl;
	type = "Weapon";
}

void Weapon::draw()
{
	Obj::draw();
			//cout << "weapon draw called\tname: " << this->name << endl;
			if(isFiring)
			{
			//	cout << "obj fire draw call ..  " << endl;
				glPushMatrix();
					glLoadIdentity();
					glTranslatef(1.2f, -0.65f, -4.4f);
					glScalef(0.15f,0.15f,0.15f);
					drawFire();

					if(firingCount == 0)
					{
						isFiring = false;
						firingCount = 25;
					}
					else
						firingCount--;

				glPopMatrix();
			}
}	
void Weapon::reposition()
{
	this->pos = finalPos;
	this->scale.x = finalScale;
	this->scale.y = finalScale;
	this->scale.z = finalScale;
	this->setRot(w,x,y,z);
}

Weapon& Weapon::operator=(const Obj& other)
{

		this->name = other.name;
		this->rot = other.rot;
		this->collision = other.collision;
		this->numOfFaces = other.numOfFaces;
		this->portAngle = other.portAngle;
		this->pos = other.pos;
		this->scale = other.scale;
		this->rotAxes = other.rotAxes;
		this->collisionVec3 = other.collisionVec3;

		return *this;	

}

void Weapon::drawFire()
{

	static GLfloat UV[] = { 0,0,	1,0,	1,1,	0,1 };
	static GLfloat vertices[] = { 0, 0, 0, 	0, 0, -16.4f,		0, 12.7f, -16.4f,		0, 12.7f, 0 };

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, fire);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, UV);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glDrawArrays(GL_QUADS, 0, 4);
	//cout << "fire drawn" << endl;

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

}