#include "Terrain.h"

Terrain::Terrain(float len, float wid) :
	length(len), width(wid), tileSize(16), heightRatio(1500.0f), scalar(10.0f)
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
void Terrain::draw(bool wireFrame)
{
	Vec3 tmpVec(0,0,0);

	if(wireFrame)
		glPolygonMode(GL_FRONT, GL_LINE);
	else
		glPolygonMode(GL_FRONT,GL_FILL);

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

	for(int i=0; i < (nSize-1); i++)
	{
		for(int j=0; j < (nSize-1); j++)
		{
			int idx0 = i + j * nSize;
			int idx1 = i + (j + 1) * nSize;
			int idx2 = i + 1 + (j + 1) * nSize;
			int idx3 = i + 1 + j * nSize;
			Vec3 p0, p1, p2, p3;

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

			Vec3 norm = returnNormal(p0,p1,p2);
			glNormal3f(norm.x,norm.y,norm.z);

			norm = returnNormal(p2,p3,p0);
			glNormal3f(norm.x,norm.y,norm.z);

		}
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
		glColor4f(Y*0.5f, 0.7f, 0.5f, 1.0f);				//water layer
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

void Terrain::draw2(bool wireFrame)
{
	Vec3 tmpVec(0,0,0);

	if(wireFrame)
	{
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
