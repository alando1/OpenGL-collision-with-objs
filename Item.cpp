#include "Item.h"

Item::Item(const char* fileName, const char* Name, Vec3& initialPos) :
	Obj(fileName, Name)
{ 
	pickupSFX = NULL;
	this->name = Name;
	this->pos = initialPos;
	//cout << "Item::Item(...), setting type to Item" << endl;
	type = "Item";
}

Item::Item()
{
	//cout << "Item::Item(), setting type to Item" << endl;
	type = "Item";
}

bool Item::isInRange(const Vec3& playerPos, float radius)
{
	if(playerPos.distance(pos) < radius)
	{
		//cout << "player is in Range... " << endl;
		if(pickupSFX != NULL)
		{
			//cout << "pickupSFX is " << pickupSFX << endl;
			Mix_PlayChannel( -1, pickupSFX, 0 );
		}
		return TRUE;
	}
	else 
		return FALSE;
}

Item& Item::operator=(const Obj& other)
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

Item::~Item()
{}