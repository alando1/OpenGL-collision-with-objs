#include "Weapon.h"

Weapon::Weapon()
{
	
}

void Weapon::drawWeapon()
{
	
}

Weapon& Weapon::operator=(const Obj& other)
{

		if(vertices != NULL)
			delete[] this->vertices;									//deletes this A array contents
		if(textures != NULL)
			delete[] this->textures;
		if(normals != NULL)
			delete[] this->normals;
		if(indices != NULL)
		delete[] this->indices;

		tmpVecArray.clear();
		tmpUVsArray.clear();
		tmpNrmArray.clear();
		tmpIndxArray.clear();

		int p = 0;
		if (other.tmpVecArray.size() > 0)
		{
			vertices = new GLfloat[other.tmpVecArray.size() * 3];
			for (auto i = other.tmpVecArray.begin(); i != other.tmpVecArray.end(); ++i)
			{
				vertices[p++] = i->x;
				vertices[p++] = i->y;
				vertices[p++] = i->z;
			}
		}

		p = 0;
		if (other.tmpUVsArray.size() > 0)
		{
			textures = new GLfloat[other.tmpUVsArray.size() * 2];
			for (auto i = other.tmpUVsArray.begin(); i != other.tmpUVsArray.end(); ++i)
			{
				textures[p++] = i->x;
				textures[p++] = i->y;
			}
		}

		p = 0;
		if (other.tmpNrmArray.size() > 0)
		{
			normals = new GLfloat[other.tmpNrmArray.size() * 3];
			for (auto i = other.tmpNrmArray.begin(); i != other.tmpNrmArray.end(); ++i)
			{
				normals[p++] = i->x;
				normals[p++] = i->y;
				normals[p++] = i->z;
			}
		}

		p = 0;
		if (other.tmpIndxArray.size() > 0)
		{
			indices = new (nothrow) GLint[other.tmpIndxArray.size()];

			if (indices != NULL)
			{
				for (auto i = other.tmpIndxArray.begin(); i != other.tmpIndxArray.end(); ++i)
				{
					indices[p++] = *i;
				}
			}
			else 
				cout << name << " indices failed to allocate" << endl;
		}

/*
		vertices = new GLfloat[other.vertices->size()];						//create new array of type T and transfer contents
		for(int i= 0; i < other.vertices->size(); ++i)
			this->vertices[i] = other.vertices[i];

		textures = new GLfloat[other.textures->size()];						//create new array of type T and transfer contents
		for(int i= 0; i < other.texture->size(); ++i)
			this->textures[i] = other.textures[i];

		normals = new GLfloat[other.normals->size()];						//create new array of type T and transfer contents
		for(int i= 0; i < other.normals->size(); ++i)
			this->normals[i] = other.normals[i];

		indices = new GLint[other.indices->size()];						//create new array of type T and transfer contents
		for(int i= 0; i < other.indices->size(); ++i)
			this->indices[i] = other.indices[i];*/

		this->textureLocation = other.textureLocation;
		this->name = other.name;
		this->objName = other.objName;
		this->rot = other.rot;
		this->texture = other.texture;
		this->collision = other.collision;
		this->numOfFaces = other.numOfFaces;
		this->portAngle = other.portAngle;
		this->pos = other.pos;
		this->scale = other.scale;
		this->rotAxes = other.rotAxes;
		this->collisionVec3 = other.collisionVec3;

		return *this;	

}