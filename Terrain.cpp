#include "Terrain.h"

Terrain::Terrain(float len, float wid) :
	length(len), width(wid), tileSize(16), heightRatio(1500.0f), scalar(10.0f), numOfVerts(0), 
	vertices(NULL), colors(NULL), normals(NULL) 
{
	heights = new float[width*length];
}

Terrain::~Terrain()
{
	delete[] heights;
}
void Terrain::loadFile(const char* fileName, int nsize)
{
	nSize = nsize;
	FILE* pFile = fopen(fileName, "rb");
	//cout << ">Error #1: " << strerror(errno) << endl;
	if(pFile == NULL)
	{
		cout << ">Error, could not open " << fileName << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		unsigned char* buffer = new unsigned char[nSize*nSize];
		int readIsGood = fread(buffer, 1, nSize*nSize, pFile);
		if(readIsGood != 0)
		{
			/*cout << ">fread worked properly." << endl;*/
			buildHeightMap(buffer);
		}
		else
		{
			cout << ">Error, could not read " << fileName << endl;
			cout << ">Error #2: " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}


		delete[] buffer;
	}
	fclose(pFile);
}	
void Terrain::draw()
{
	Vec3 tmpVec(0,0,0);

	glPolygonMode(GL_FRONT, GL_LINE);

	glBegin(GL_TRIANGLES);

	for(int i=0; i < (nSize-1); i++)
	{
		for(int j=0; j < (nSize-1); j++)
		{
			
			setVertexColor(i, j);
			setVertexColor(i, j + 1);
			setVertexColor(i + 1, j + 1);

			setVertexColor(i + 1, j + 1);
			setVertexColor(i + 1, j);
			setVertexColor(i, j);
		}
	}
	glEnd();
}

void Terrain::normalizeTerrain()
{
	Vec3 tmpVec(0,0,0);
	int idx0, idx1, idx2, idx3;
	Vec3 p0, p1, p2, p3;
	int p = 0;

	for(int i=0; i < (nSize-1); i++)
	{
		for(int j=0; j < (nSize-1); j++)
		{
			idx0 = i + j * nSize;
			idx1 = i + (j + 1) * nSize;
			idx2 = i + 1 + (j + 1) * nSize;
			idx3 = i + 1 + j * nSize;

			p0.x = tileSize*scalar*4.0f*i;
			p0.y = scalar*(heightRatio*heights[idx0])-190.0f;
			p0.z = tileSize*scalar*4.0f*j;

			p1.x = tileSize*scalar*4.0f*i;
			p1.y = scalar*(heightRatio*heights[idx1])-190.0f;
			p1.z = tileSize*scalar*4.0f*(j+1);

			p2.x = tileSize*scalar*4.0f*(i+1);
			p2.y = scalar*(heightRatio*heights[idx2])-190.0f;
			p2.z = tileSize*scalar*4.0f*(j+1);

			p3.x = tileSize*scalar*4.0f*(i+1);
			p3.y = scalar*(heightRatio*heights[idx3])-190.0f;
			p3.z = tileSize*scalar*4.0f*j;

			tmpColArray.push_back(setColorArray(heights[idx0]));
			tmpColArray.push_back(setColorArray(heights[idx1]));
			tmpColArray.push_back(setColorArray(heights[idx2]));

			tmpColArray.push_back(setColorArray(heights[idx2]));
			tmpColArray.push_back(setColorArray(heights[idx3]));
			tmpColArray.push_back(setColorArray(heights[idx0]));


			tmpVecArray.push_back(Vec3(p0.x,p0.y,p0.z));
			tmpVecArray.push_back(Vec3(p1.x,p1.y,p1.z));
			tmpVecArray.push_back(Vec3(p2.x,p2.y,p2.z));
			tmpVecArray.push_back(Vec3(p2.x,p2.y,p2.z));
			tmpVecArray.push_back(Vec3(p3.x,p3.y,p3.z));
			tmpVecArray.push_back(Vec3(p0.x,p0.y,p0.z));
			numOfVerts+=6;

			Vec3 norm = returnNormal(p0,p1,p2);
			tmpNrmArray.push_back(norm);
			tmpNrmArray.push_back(norm);
			tmpNrmArray.push_back(norm);
			//			glNormal3f(norm.x,norm.y,norm.z);

			norm = returnNormal(p2,p3,p0);
			tmpNrmArray.push_back(norm);
			tmpNrmArray.push_back(norm);
			tmpNrmArray.push_back(norm);
//			glNormal3f(norm.x,norm.y,norm.z);
		}
	}

	vertices = new GLfloat[tmpVecArray.size() * 3];
	for (auto i = tmpVecArray.begin(); i != tmpVecArray.end(); ++i)
	{
		vertices[p++] = i->x;
		vertices[p++] = i->y;
		vertices[p++] = i->z;
	}

	p = 0;
	normals = new GLfloat[tmpNrmArray.size() * 3];
	for (auto i = tmpNrmArray.begin(); i != tmpNrmArray.end(); ++i)
	{
		normals[p++] = i->x;
		normals[p++] = i->y;
		normals[p++] = i->z;
	}

	p = 0;
	colors = new GLfloat[tmpColArray.size() * 4];
	for (auto i = tmpColArray.begin(); i != tmpColArray.end(); ++i)
	{
		colors[p++] = i->r;
		colors[p++] = i->g;
		colors[p++] = i->b;
		colors[p++] = i->a;
	}
}

Vec3 Terrain::returnNormal(Vec3& x,Vec3& y,Vec3& z)
{
	Vec3 v1 = y - x;
	Vec3 v2 = z - x;

	return v1.cross(v2).normalize();
}

void Terrain::setVertexColor(int x, int z)
{

	int index = z * nSize + x;
	float Y = heights[index];
	float X = tileSize * x;
	float Z = tileSize * z;

	if(Y<0.1f)												
		glColor4f(Y*0.5f, 0.7f, 0.5f, 0.5f);				//water layer
	else if(Y>0.1f && Y<0.3f)								
		glColor4f(Y*0.5f, 0.156f+Y*0.2f, Y*0.3f, 1.0f);		//green layer
	else if(Y>0.50f && Y<0.54f)								
		glColor4f(Y-0.2f, Y-0.2f, 0.1f, 1.0f);				//gold rock layer
	else if(Y>0.65f && Y<=0.8f)																	
		glColor4f(Y*0.371f, Y/4.67f, Y*(0.1f-Y), 1.0f);		//red rock blend color
	else if(Y>0.8f)											
		glColor4f(Y-0.1f, Y-0.2f, Y-0.3f, 1.0f);			//snow layer
	else
		glColor4f(Y*0.50f, 0.156f, 0.0f, 1.0f);
															//glColor3f(Y, Y*0.7f, Y*0.2f);//Sand Mountain

	glVertex3f(4*scalar*X,(scalar*(heightRatio*Y)-190.0f),4*scalar*Z);


	//remove 4's for 1024 by 1024 ratio images
}

void Terrain::draw2()
{
	Vec3 tmpVec(0,0,0);

	glPolygonMode(GL_FRONT,GL_FILL);

	glBegin(GL_TRIANGLES);

	for(int i=0; i < (nSize-1); i++)
	{
		for(int j=0; j < (nSize-1); j++)
		{
			setVertexColor2(i, j);
			setVertexColor2(i, j + 1);
			setVertexColor2(i + 1, j + 1);
			setVertexColor2(i + 1, j + 1);
			setVertexColor2(i + 1, j);
			setVertexColor2(i, j);
		}
	}
	glEnd();
	
}

void Terrain::setVertexColor2(int x, int z)
{

	int index = z * nSize + x;
	float Y = heights[index];
	float X = tileSize * x;
	float Z = tileSize * z;

	glColor4f(0,0,0,0.5f);//Sand Mountain
	glVertex3f(4*scalar*X,(scalar*(heightRatio*Y)-190.0f),4*scalar*Z);
}


void Terrain::buildHeightMap(unsigned char* buffa)
{
//	cout << "> Building Height Map ..." << endl;
	auto start = chrono::steady_clock::now();

	for(int i=0; i < (nSize*nSize); i++)
	{
		heights[i] = static_cast<float>(buffa[i])/255.0f;
	}
	auto finish = chrono::steady_clock::now();
	float dt = chrono::duration_cast<chrono::duration<float, milli>>(finish-start).count();

	//cout << "> Height Map built in " << dt << " milliseconds." << endl;

	this->normalizeTerrain();
}

float* Terrain::returnHeights()
{
	return heights;
}

float Terrain::scale()
{
	return scalar;
}

void Terrain::drawElements()
{

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	

	if (colors != NULL)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colors);
	}
		

	if (normals != NULL)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, normals);
	}


   // glColor4f(1,1,1,1);
	glDrawArrays(GL_TRIANGLES, 0, numOfVerts);

    glDisableClientState(GL_VERTEX_ARRAY);  			  // disable vertex array
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);		 // disable texture array
    glDisableClientState(GL_NORMAL_ARRAY);			// disable normal array
    glDisableClientState(GL_COLOR_ARRAY);			// disable color array
}

color4 Terrain::setColorArray(float Y)
{
	color4 ret;

	if(Y<0.1f)												
	{
		ret.r = Y * 0.5f;
		ret.g = 0.7f;
		ret.b = 0.5f;
		ret.a = 0.5f;
	}
	else if(Y>0.1f && Y<0.3f)		
	{
		ret.r = Y * 0.5f;
		ret.g = 0.156f+Y*0.2f;
		ret.b = Y*0.3f;
		ret.a = 1.0f;
	}						
	else if(Y>0.50f && Y<0.54f)		
	{
		ret.r = Y-0.2f;
		ret.g = Y-0.2f;
		ret.b = 0.1f;
		ret.a = 1.0f;
	}	
	else if(Y>0.65f && Y<=0.8f)																	
	{
		ret.r = Y*0.371f;
		ret.g = Y/4.67f;
		ret.b = Y*(0.1f-Y);
		ret.a = 1.0f;
	}

	else if(Y>0.8f)								
	{
		ret.r = Y-0.1f;
		ret.g = Y-0.2f;
		ret.b = Y-0.3f;
		ret.a = 1.0f;
	}			
	else
	{
		ret.r = Y*0.50f;
		ret.g = 0.156f;
		ret.b = 0.0f;
		ret.a = 1.0f;
	}

		return ret;
}
