#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <stdio.h>
#include <string>
#include <chrono>
/*---------------*/
#include "DrawFunc.h"
#include "suppliedGlutFuncs.h"
#include "Terrain.h"
#include "Vec3.h"
#include "Portal.h"
#include "Bullet.h"
#include "Obj.h"
#include "Weapon.h"
#include "Item.h"
#include "Mesh.h"
/*---------------*/
#include <SOIL/SOIL.h>
#include <SDL/SDL.h>
#undef main 	//required by SDL
#include <SDL/SDL_mixer.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

/*#define true 1
#define false 0*/
#define GRAVITY -981.0f
#define imageSize 256

/*-----Points-in-Time-----------------------------------------------------------*/
chrono::steady_clock::time_point currentTime, lastTime, jumpStart, portalPassTime;
chrono::steady_clock::time_point lastWarp, shootStart, shootEnd;

/*-------Objects----------------------------------------------------------------*/
Obj* stormtrooper = NULL;
Obj* building = NULL;
Obj* pistol = NULL;
Obj* pist2 = NULL;
Obj* tree = NULL;
Obj* elephant = NULL;
Obj* portalGun = NULL;
Obj* meshTree = NULL;
Obj* nimbus = NULL;

/*------Items-------------------------------------------------------------------*/
Item* iPistol = NULL;
Item* iPortal = NULL;

Weapon* wPistol = NULL;
Weapon* wPortal = NULL;
Weapon* assault = NULL;

/*------Bool-Inputs-------------------------------------------------------------*/
bool readyToWarp = true;
bool keyStates[256], keyTaps[256];
bool isCrouched = false;
bool inJump = false;
bool inFirstWorld = true;
bool firingWeapon = false;
bool hasPortalGun = false;
bool hasPistol = false;
bool zoom = false;
bool wireFrame = false;
bool inBounds = true;
bool holdingPortalGun = false;
bool holdingPistol = false;

/*------View-Angles-------------------------------------------------------------*/
float heading = 0.0f;
float pitch = 0.0f;
float perspective = 45.0f;
float aspectRatio = 1024.0f/768.0f;	

/*------Frame-Data--------------------------------------------------------------*/
float FPS = 0.0f;
float fpsUpdateTime = 0.0f;
int frameCount = 0;

/*--Camera Positions and Look Vectors--------------------------------------------*/
Vec3 camLook(0.0f,0.0f,-1.0f);
Vec3 camPos(50058.21f, 12144.12f, 95300.75f);//camPos(1725.0f,5.0f,4308.0f);
Vec3 portalGunPos(camPos.x, camPos.y, camPos.z-1000);
Vec3 pistolPos(camPos.x+1000, 14825, camPos.z-1000);
Vec3 assaultPos(pistolPos.x-3000, pistolPos.y, pistolPos.z);
Vec3 stormPos(portalGunPos.x, 14825, portalGunPos.z-5000);
Vec3 yodaPos(portalGunPos.x, 14825, portalGunPos.z-10000);
Vec3 respawnPos(50058.21f, 121441.20f, 95300.75f);
Vec3 lightPos(50058.21f, 16830.0f, 90000.0f);

GLfloat light_position[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };

/*------Velocities--------------------------------------------------------------*/
float dt;
float speed, flySpeed=0.15;
float updateSpeedTime = 0.0f;
float camSpeed = 0.04f;
float addCrouch = 0.0f;
float crouchFactor = 1.0f;
float jumpVelocity = 3000.0f;//1500.0f;//200.0f;
float initialJumpHeight = 0.0f;
Vec3 prevPos=camPos;

/*------Texture Data------------------------------------------------------------*/
GLUquadricObj* quadric;
int centerX, centerY, texture, sun;
void* font = GLUT_BITMAP_HELVETICA_18;
bool loadHiRes;


/*------Portals-----------------------------------------------------------------*/
list<Portal*> portalList;
int PortalsCenter, PortalsOuter, Portals;
float iconSpin = 1;

/*------Sounds------------------------------------------------------------------*/
Mix_Music *gMusic = NULL;
Mix_Chunk *shootSFX = NULL;
Mix_Chunk *gSkyDive = NULL;
Mix_Chunk *gWrecked = NULL;
Mix_Chunk *gShowMe = NULL;
Mix_Chunk *gBlap = NULL;
Mix_Chunk *gPistol = NULL;

/*------Terrain-Data------------------------------------------------------------*/
float* currentMap;
float* mapHeights;
float* mapHeights1;
float scale;
Terrain terrain(imageSize,imageSize);
Terrain terrain1(imageSize,imageSize);
Terrain terrain2(imageSize,imageSize);
Terrain terrain3(imageSize,imageSize);
int xt = 0; int zt = 0; int indx = 0;

/*------bullet------------------------------------------------------------------*/
Bullet* ball;
list<Bullet*> bulletList;
list<Obj*> objList;
vector<Item*> items;
Weapon* currentWeapon = NULL;
Weapon* prevWeapon = NULL;
Weapon* nextWeapon = NULL;
int myWeapon = -1;
vector<Weapon*> myWeapons;
vector<Weapon*> weaponList;

void background()
{
	/*---Clear color and Depth Buffers----------------------------------------------*/
	glClearColor(0.5f,0.5f,0.5f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		gluLookAt(0,0,0,camLook.x,camLook.y,camLook.z,0,1,0);
			glDisable(GL_LIGHTING);
			glDisable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);

				glColor3f(1,1,1);
				glTranslatef(500.0f,500.0f,-1500.0f);
				glRotatef(90, 1,0,0);
				texturePlane(texture, 496.0f, 692.0f);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_LIGHTING);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDepthMask(GL_TRUE);
}
void ballOfLight()
{
	/*----Ball of Light-------------------------------------------------------------*/
	GLfloat light_position[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glPushMatrix();
		glTranslatef(lightPos.x,lightPos.y-500,lightPos.z);
		glPushMatrix();
			glRotatef(iconSpin, 0,1,0);
			glTranslatef(1000,0,0);
			glColor4f(1,1,0,0.2f);

			glutSolidSphere(200, 15, 15);
		glPopMatrix();
	glPopMatrix();
	glColor4f(1,1,1,1);

	/*----end of Ball of Light------------------------------------------------------*/
}
void updateCamHeight()
{
	zt = static_cast<int>(camPos.z)/(64*scale);
	xt = static_cast<int>(camPos.x)/(64*scale); 
		if(zt<0 || xt<0)
		return;
	indx = zt * imageSize + xt;
	if(camPos.y < (currentMap[indx] * 1500.0f - 2000.0f))
		return;
	else
		camPos.y = scale * currentMap[indx] * 1500.0f;
}
bool floorDistance()
{
	zt = static_cast<int>(camPos.z)/(64*scale);
	xt = static_cast<int>(camPos.x)/(64*scale); 
	if(zt<0 || xt<0)
		return false;
	indx = zt * imageSize + xt;
	if(camPos.y > (scale * currentMap[indx] * 1500.0f - (25.0f )))// - 150.0f))
		return false;
	else if(camPos.y < (currentMap[indx] * 1500.0f - (scale * 300.0f)))
		return false;
	else
		return true;
}
void handleFunc(float dt)
{
	if(camPos.x >= 0 && camPos.z >= 0)
		if(floorDistance()) 
			updateCamHeight();

	float vecx, vecy, vecz;

	if(keyTaps['1'])
		inFirstWorld = !inFirstWorld;

	/*-----Zoom---------------------------------------------------------------------*/
	if(zoom)
	{
		if(perspective <= 2.0f)
		{
			perspective = 5.0f;
			zoom = false;
		}
		else
		{
			perspective -= 0.05f*dt;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 400000.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	/*--------change perspective----------------------------------------------------*/
	if(keyStates['-'])
	{
		perspective -= 1.0f;
		if(perspective <= 1.0f)
			perspective = 180.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 400000.0f);
		glMatrixMode(GL_MODELVIEW);
	}
	if(keyStates['\\'])
	{
		perspective= 45.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 400000.0f);
		glMatrixMode(GL_MODELVIEW);
	}
	if(keyStates['='])
	{
		perspective += 1.0f;
		if(perspective >= 180.0f)
			perspective = 0.0f;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(perspective, aspectRatio, 0.1f, 400000.0f);
		glMatrixMode(GL_MODELVIEW);
	}

	/*---------Fly-Foward-----------------------------------------------------------*/
	if(keyStates['q'] || keyStates['Q'])
		camPos+=camLook*(flySpeed*50.0f*dt);

	if (keyTaps['q'] || keyTaps['Q'])
		Mix_PlayChannel( 3, gSkyDive, -1 );

	/*--------Walk-Foward-----------------------------------------------------------*/
	if(keyStates['w'] || keyStates['W'])
	{

		camPos.x+=camLook.x*(crouchFactor*camSpeed*50.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*50.0f*dt);
	}

	/*---------sprint---------------------------------------------------------------*/
	if(keyStates['r'] || keyStates['R'])
	{
		camPos.x+=camLook.x*(crouchFactor*camSpeed*50.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*50.0f*dt);
	}

	/*--------Move-Backwards--------------------------------------------------------*/
	if(keyStates['s'] || keyStates['S'])
	{
		camPos.x-=camLook.x*(crouchFactor*camSpeed*50.0f*dt);
		//y-=ly*speed;  //comment out to walk backwards on plane
		camPos.z-=camLook.z*(crouchFactor*camSpeed*50.0f*dt);
	}

	/*--------Move-Left-------------------------------------------------------------*/
	if(keyStates['a'] || keyStates['A'])
	{
		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x+= vecx*(crouchFactor*camSpeed*50.0f*dt);
		camPos.z+= vecz*(crouchFactor*camSpeed*50.0f*dt);
	}

	/*--------Move-Right------------------------------------------------------------*/
	if(keyStates['d'] || keyStates['D'])
	{
		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x-= vecx*(crouchFactor*camSpeed*50.0f*dt);
		camPos.z-= vecz*(crouchFactor*camSpeed*50.0f*dt);		
	}

	/*---------crouch---------------------------------------------------------------*/
	if(keyTaps['c'] || keyTaps['C'])
	{
		if(!isCrouched)
		{
			addCrouch = -120.0f;
			crouchFactor = 0.45f;
		}
		else 
		{
			addCrouch = 0.0f;
			crouchFactor = 1.0f;
		}
		isCrouched = !isCrouched;
	}

	/*----------up/down-------------------------------------------------------------*/
	if(keyStates['x'] || keyStates['X'])
		camPos.y += (crouchFactor*camSpeed*13.0f*dt);

	if(keyStates['Z'] || keyStates['z'])
		camPos.y -= (crouchFactor*camSpeed*13.0f*dt);

	/*----------jump----------------------------------------------------------------*/
	if(keyStates[' '])
	{
		inJump = true;
		initialJumpHeight = camPos.y;
		jumpStart = chrono::steady_clock::now();
	}
	if(inJump)
	{	
		auto now = chrono::steady_clock::now();
		float tmpTime = chrono::duration_cast<chrono::duration<float, milli>>(now-jumpStart).count();
		tmpTime = tmpTime/1000.0f;

		float tmpVelocity = jumpVelocity + GRAVITY * tmpTime;
		camPos.y = initialJumpHeight + tmpVelocity*tmpTime + ((0.5f) * (GRAVITY) * (tmpTime) * (tmpTime));

		if(xt < 0 || zt < 0)
			inBounds = false;

		if(camPos.y <= (scale * currentMap[indx] * 1500.0f) && inBounds)
		{
			camPos.y = scale * currentMap[indx] * 1500.0f;
			inJump = false;
		}
		else if(!inBounds)
		{
			if(camPos.y <= -5000.0f)
			{
				camPos = respawnPos;
				inJump = true;
				inBounds = true;
			}
		}
	}

	/*-----Toggle wireframe---------------------------------------------------------*/
	if(keyTaps['t'] || keyTaps['T'])
	{
		wireFrame = !wireFrame;

		if(wireFrame)
			glPolygonMode(GL_FRONT, GL_LINE);
		else
			glPolygonMode(GL_FRONT,GL_FILL);
	}

	/*-----teleport to 000---------------------------------------------------------*/
	if(keyTaps['\t'])
	{
		Vec3 tmp(0,0,0);
		camPos = tmp;
	}


	/*-----Light Movement code------------------------------------------------------*/
		if(keyStates['j'] || keyStates['J'])
	{
		lightPos.x-=(5.0f*dt);
	}
	if(keyStates['k'] || keyStates['K'])
	{
		lightPos.z+=(5.0f*dt);
	}
	if(keyStates['l'] || keyStates['L'])
	{
		lightPos.x+=(5.0f*dt);
	}
	if(keyStates['i'] || keyStates['I'])
	{
		lightPos.z-=(5.0f*dt);
	}
	if(keyStates['o'] || keyStates['O'])
	{
		lightPos.y+=(5.0f*dt);
	}
	if(keyStates['p'] || keyStates['P'])
	{
		lightPos.y-=(5.0f*dt);
	}
}
void mapItem()
{
	//initial position of mapItem
	int tmpzt = static_cast<int>(camPos.z - 1000)/(64*scale);
	int tmpxt = static_cast<int>(camPos.x)/(64*scale); 
	indx = tmpzt * imageSize + tmpxt;
	portalGunPos.y = scale * currentMap[indx] * 1500.0f;
	pistolPos.y = portalGunPos.y;
}
void finalUpdates()
{
	/*----Update Frames-------------------------------------------------------------*/
	if (fpsUpdateTime > 100.0f)
	{
		FPS = static_cast<float>(frameCount) / fpsUpdateTime * 1000.0f;
		frameCount = 0;
		fpsUpdateTime = 0.0f;
	}

	/*----Update prevPos------------------------------------------------------------*/
	prevPos = camPos;		

	memset(keyTaps,0,256*sizeof(bool));

    printScreenText();
    frameCount++;
	glutSwapBuffers();
}
void updateTime()
{
	/*---update time every frame----------------------------------------------------*/
	iconSpin += (360.0f * dt / 1000.0f);						//spins 2pi per sec
	currentTime = chrono::steady_clock::now();
	dt = chrono::duration_cast<chrono::duration<float, milli>>(currentTime-lastTime).count();
    lastTime = currentTime;
    fpsUpdateTime += dt;
	speed = camPos.distance(prevPos) * dt / 1000.0f;
}
void updatePortals()
{
	list<Portal*>::iterator it;
	auto now = chrono::steady_clock::now();

	bool refresh = true;
	
	while(refresh)
	{
		refresh = false;
		for(it = portalList.begin(); it!=portalList.end(); ++it)
		{
			auto end = (*it)->End();
			if(end < now)
			{
				portalList.erase(it);
				refresh = true;
				break;
			}
		}
	}

	it = portalList.begin();
	while(it != portalList.end())
	{
		for(it = portalList.begin(); it!=portalList.end(); ++it)
		{
			//get distance of camPos and portalPos
			Vec3 tmp = (*it)->returnPosition();
			Vec3 r = camPos-tmp;

			if(!readyToWarp)
			{
				auto endWarp = lastWarp + chrono::milliseconds(2000);
				auto tmpnow = chrono::steady_clock::now();

				if(endWarp < tmpnow)
					readyToWarp = true;
			}

			auto nowish = chrono::steady_clock::now();
			//within bounds
			if((r.length()<200.0f) && readyToWarp == true && (*it)->Open() < nowish)
			{
					inFirstWorld = !inFirstWorld;
					if(inFirstWorld)
						currentMap = mapHeights;
					else
						currentMap = mapHeights1;

				readyToWarp = false;
				lastWarp = chrono::steady_clock::now();

				return;
			}
		}
	}	
}
void updateBullets()
{
	list<Bullet*>::iterator it;
	list<Obj*>::iterator it2;
	auto now = chrono::steady_clock::now();

	bool refresh = true;
	
	while(refresh)
	{
		refresh = false;
		for(it = bulletList.begin(); it!=bulletList.end(); ++it)
		{
			auto end = (*it)->End();
			if(end < now)
			{
				bulletList.erase(it);
				refresh = true;
				break;
			}
		}
	}

	it = bulletList.begin();
	Vec3 r;
	while(it != bulletList.end())
	{
		for(it = bulletList.begin(); it!=bulletList.end(); ++it)
		{
			Vec3 tmp = (*it)->returnPos();

			for(it2 = objList.begin(); it2!=objList.end(); ++it2)
			{
				if(true)
				{
					r = (*it2)->returnPos()-tmp;
				}

				if(r.length() < 500.0f)
				{
					(*it2)->collision = true;
					(*it2)->collisionVec3 = (*it)->returnLook();
				}
				
			}	
		}
	}
}
void updateWeapons()
{
	currentWeapon = NULL;
	bool refresh = true;
	while(refresh)
	{
		refresh = false;
		for(auto i = weaponList.begin(); i != weaponList.end(); i++)
		{
			currentWeapon = *i;
			if(currentWeapon->isInRange(camPos, 200))
			{
				cout << ">Weapon " << currentWeapon->getName() << " is in range." << endl;
				Weapon* tmp;
				tmp = currentWeapon;
				tmp->reposition();

				myWeapons.push_back(tmp);
				weaponList.erase(i);
				myWeapon = myWeapons.size() - 1;
				cout << ">Weapon " << currentWeapon->getName() << " was added to inventory." << endl;
				refresh = true;
				break;
			}
		}
	}
}
void renderPortals()
{
	list<Portal*>::iterator it;
	for(it = portalList.begin(); it!=portalList.end(); ++it)
		(*it)->openPortal();
}
void renderBullets()
{
	list<Bullet*>::iterator it;
	for(it = bulletList.begin(); it!=bulletList.end(); ++it)
		(*it)->draw();
}
void drawPickups()
{
	vector<Weapon*>::iterator it;
	for(it = weaponList.begin(); it!=weaponList.end(); ++it)
		(*it)->draw();
}
void drawPlayerWeapon()
{
	if(myWeapon != -1)
	{
		glLoadIdentity();
			glPushMatrix();
				myWeapons[myWeapon]->draw();
			glPopMatrix();
	}
}
void updateLists()
{
	updateTime();
	if(!portalList.empty())
		updatePortals();
	if(!bulletList.empty())
		updateBullets();
	if(!weaponList.empty())
		updateWeapons();
}
void renderLists()
{
	renderPortals();
	renderBullets();
	drawPickups();
	drawPlayerWeapon();
}
void firstWorldDraw()
{
		terrain.drawElements();

		glEnable(GL_TEXTURE_2D);
		glPushMatrix();

			glPushMatrix();
				building->draw();
			glPopMatrix();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL, 0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPushMatrix();
				float tmpTreez = meshTree->pos.z;
			for(int z = 0; z < 10; z++)
				{
					meshTree->draw();
					meshTree->pos.x += 1200.0f;
					meshTree->draw();

					meshTree->pos.x -= 1200.0f;
					meshTree->pos.z -= 5000.0f;
				} 
				meshTree->pos.z = tmpTreez;
			glPopMatrix();
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);

			glPushMatrix();
				if(stormtrooper->collision == true)
					stormtrooper->setRot(iconSpin,stormtrooper->collisionVec3.x,0,stormtrooper->collisionVec3.z);

				stormtrooper->draw();
			glPopMatrix();

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
}
void secondWorldDraw()
{
	terrain1.drawElements();
/*	glPushMatrix();
		nimbus->draw();
	glPopMatrix();*/
}
void renderScene(void)
{
	updateLists();
	handleFunc(dt);
	background();

	/*----Draw objects in 3d scene--------------------------------------------------*/
	// Reset transformations
	glLoadIdentity();
	gluLookAt(camPos.x,camPos.y+addCrouch,camPos.z,camPos.x+camLook.x,camPos.y+addCrouch+camLook.y,camPos.z+camLook.z,0,1,0);

	ballOfLight();

	if(inFirstWorld)
		firstWorldDraw();
	else
		secondWorldDraw();

	glDisable(GL_CULL_FACE);

	renderLists();
	finalUpdates();
}
void initAudio()
{

	/*---Initialize SDL_audio-------------------------------------------------------*/
	if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
        printf( ">SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    else
    	cout << ">SDL_audio initialized." << endl;

	/*---Initialize SDL_mixer-------------------------------------------------------*/
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
        printf( ">SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    else
    	cout << ">SDL_mixer initialized." << endl;
    
	/*---Game Music-----------------------------------------------------------------*/
	//gMusic = Mix_LoadMUS("./SFX/SpaceMusic.mp3");
      gMusic = Mix_LoadMUS("./SFX/8bit.wav");
    //gMusic = Mix_LoadMUS( "./SFX/MaculateEsseks.mp3" );
    Mix_PlayMusic( gMusic, -1 );

    /*--------Sound FX--------------------------------------------------------------*/
    gSkyDive = Mix_LoadWAV("./SFX/Skydive.wav");
    shootSFX = Mix_LoadWAV("./SFX/portalBlast.wav");
    gWrecked = Mix_LoadWAV("./SFX/riggity.wav");
    gBlap = Mix_LoadWAV("./SFX/blap3.wav");
    gShowMe = Mix_LoadWAV("./SFX/showMeWhatYouGot.wav");
    gPistol = Mix_LoadWAV("./SFX/pistol.wav");
    Mix_PlayChannel( 3, gShowMe, 0);
}
void load()
{
	/*----Load Texture images-------------------------------------------------------*/
	texture = SOIL_load_OGL_texture("./textures/showMeWhatYouGot.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	Portals = SOIL_load_OGL_texture("./textures/portal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	PortalsCenter = SOIL_load_OGL_texture("./textures/portalcenter.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	PortalsOuter = SOIL_load_OGL_texture("./textures/spiral2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	sun = SOIL_load_OGL_texture("./textures/Sun1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	/*----End of Load Texture images------------------------------------------------*/

	cout << ">Loading .raw terrains ... " << endl;
	/*----Load Terrains-------------------------------------------------------------*/
	terrain.loadFile("./heightMaps/wall.raw", imageSize);
	terrain1.loadFile("./heightMaps/gta.raw", imageSize);
	/*----End of Load Terrains------------------------------------------------------*/
	cout << ">Terrains loaded." << endl << endl;

	/*----Map array of indxed heights----------------------------------------------*/
	mapHeights = new float[imageSize*imageSize];
	mapHeights = terrain.returnHeights();
	currentMap = mapHeights;
	scale = terrain.scale();

	mapHeights1 = new float[imageSize*imageSize];
	mapHeights1 = terrain1.returnHeights();
	mapItem();
	/*----End of Map array of indxed heights---------------------------------------*/


	/*---set initial camheight y----------------------------------------------------*/
	int tmpx = static_cast<int>(camPos.z)/(64*scale);
	int tmpz = static_cast<int>(camPos.x)/(64*scale); 
	indx = tmpz * imageSize + tmpx;
	camPos.y = scale * currentMap[indx] * 1500.0f;
	/*---End of set initial camheight y---------------------------------------------*/

	Vec3 tmpPos(1.25f, -0.25f, -4.0f);
	Vec3 tmpPos2(0.15f, -0.07f, -0.3f);



	/*----Load Object files---------------------------------------------------------*/
	cout << ">Loading Objects ..." << endl;
	meshTree = new Obj("./data/Tree/Tree.obj", "tree");
	building = new Obj("./data/City/City.obj", "city #1");
//	nimbus = new Obj("./data/Nimbasa-City/OBJ/Nimbasa.obj", "city #2");
	wPortal = new Weapon("", "portalGun", portalGunPos, tmpPos2, 700, 1, 180, 0, 1, 0, NULL);
	wPistol = new Weapon("./data/newGun/newgun.obj", "pistol", pistolPos, tmpPos, 85, 1, 90, 0, 1, 0, "./data/newGun/handgun_Fire.png");
	assault = new Weapon("./data/AK47/AK47.obj", "auto", assaultPos, tmpPos, 85, 1, 180, 0, 1, 0, "./data/newGun/handgun_Fire.png");
	stormtrooper = new Item("./data/stormtrooper/Stormtrooper.obj", "person", stormPos);

	cout << ">All objects constructed." << endl << endl;

	weaponList.push_back(wPistol);
	weaponList.push_back(wPortal);
	weaponList.push_back(assault);
	objList.push_back(stormtrooper);


	stormtrooper->setScale(85);

	building->setPos(portalGunPos.x, 4825.0f, portalGunPos.z-5000);
	building->setScale(40);

/*	nimbus->setPos(portalGunPos.x, 4825.0f, portalGunPos.z-5000);
	nimbus->setScale(40);*/

	meshTree->setPos(portalGunPos.x - 200, 14825, portalGunPos.z-5000);
	meshTree->setScale(200);

    cout << ">Setting weapon audio ..."<< endl;
    wPistol->pickupSFX = gBlap;
    wPortal->pickupSFX = gWrecked;
    cout << ">Weapon audio enabled." <<endl;
	/*----End of Load Object files--------------------------------------------------*/


	lastTime = chrono::steady_clock::now();
}
int main(int argc, char **argv)
{
	// initialize GLUT, create window, initialize SDL
	initGL(argc, argv);
	initAudio();
	load();

	glutMainLoop();
	glutSetCursor(GLUT_CURSOR_INHERIT);

	return 1;
}