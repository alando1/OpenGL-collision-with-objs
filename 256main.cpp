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
#include "Player.h"
/*---------------*/
#include <SOIL.h>
#include <SDL.h>
#undef main 	//required by SDL
#include <SDL_mixer.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

#define GRAVITY -200.0f
#define imageSize 256

/*-----Points-in-Time-----------------------------------------------------------*/
chrono::steady_clock::time_point currentTime, lastTime, jumpStart, portalPassTime;
chrono::steady_clock::time_point lastWarp, shootStart, shootEnd;

/*-------Objects----------------------------------------------------------------*/
Obj* stormtrooper = NULL;
Obj* building = NULL;
Obj* pistol = NULL;
Obj* pist2 = NULL;
Obj* elephant = NULL;
Obj* portalGun = NULL;
Obj* nimbus = NULL;

/*------Items-------------------------------------------------------------------*/
Weapon* wPistol = NULL;
Weapon* wPortal = NULL;
Weapon* assault = NULL;
Weapon* rocket = NULL;
Item* rock = NULL;

/*------Bool-Inputs-------------------------------------------------------------*/
bool readyToWarp = true;
bool keyStates[256], keyTaps[256];
bool isCrouched = false;
bool inJump = false;
bool inFirstWorld = true;
bool zoom = false;
bool wireFrame = false;
bool inBounds = true;
bool fog = true;

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
Vec3 camLook(0.0f, 0.0f, 1.0f);
Vec3 camPos(3800, 20, 100);
Vec3 portalGunPos(camPos.x, camPos.y, camPos.z+100);
Vec3 pistolPos(camPos.x+40, camPos.y, camPos.z+100);
Vec3 assaultPos(camPos.x+20, camPos.y, camPos.z+100);
Vec3 rocketPos(camPos.x+60, camPos.y, camPos.z+100);
Vec3 stormPos(portalGunPos.x, camPos.y, portalGunPos.z+50);
Vec3 respawnPos(3800,20,100);
Vec3 lightPos(1000.0f, 1000.0f, 1000.0f);
Vec3 playerPos(0, 0, 0);

/*------Velocities--------------------------------------------------------------*/
float dt;
float speed, flySpeed=0.15;
float updateSpeedTime = 0.0f;
float camSpeed = 0.04f;
float addCrouch = 0.0f;
float crouchFactor = 1.0f;
float jumpVelocity = 300.0f;
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
Mix_Music* gMusic = NULL;
Mix_Chunk* shootSFX = NULL;
Mix_Chunk* gSkyDive = NULL;
Mix_Chunk* gWrecked = NULL;
Mix_Chunk* gShowMe = NULL;
Mix_Chunk* gBlap = NULL;
Mix_Chunk* gPistol = NULL;
Mix_Chunk* gAssualt = NULL;
Mix_Chunk* gRocket = NULL;

/*------Terrain-Data------------------------------------------------------------*/
Terrain terrain(imageSize,imageSize);
Terrain terrain1(imageSize,imageSize);
Terrain* currentTerrain = NULL;

/*------bullet------------------------------------------------------------------*/
Bullet* ball;
list<Bullet*> bulletList;
list<Obj*> objList;
list<Player*> enemyList;
//vector<Item*> items;
Weapon* currentWeapon = NULL;
int myWeapon = -1;
vector<Weapon*> myWeapons;
vector<Weapon*> weaponList;

Player* enemy = NULL;
Player* myPlayer = NULL;
int numOfZombies = 25;
void drawBackground()
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

	glPushMatrix();
		glTranslatef(lightPos.x,lightPos.y,lightPos.z);
		glPushMatrix();
			glRotatef(iconSpin/20, 0,1,0);
			glTranslatef(10000,0,0);
			glColor4f(1,1,0,0.2f);

			float light_pos0[] = {0.0f, 0.0f, 0.0f, 1.0f};
			glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
			glutSolidSphere(200, 15, 15);
		glPopMatrix();
	glPopMatrix();
	glColor4f(1,1,1,1);

	/*----end of Ball of Light------------------------------------------------------*/
}
void updateCamera()
{
	if(currentTerrain->isInBounds(camPos))
	{
			if(inJump == false)
				camPos.y = currentTerrain->getSample(camPos) + myPlayer->eye();

			if(myPlayer->isAlive())
			{
				myPlayer->pos.y = camPos.y - myPlayer->eye();
				myPlayer->pos.x = camPos.x;
				myPlayer->pos.z = camPos.z;
			}
	}

	myPlayer->setLook(camLook);
}
void handleFunc(float dt)
{
	float vecx, vecz;

	if(keyTaps['1'])
		inFirstWorld = !inFirstWorld;

	if(keyTaps['f'] || keyTaps['F'])
	{
		fog = !fog;
		if(fog)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);

	}

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
		camPos+=camLook*(flySpeed*5.0f*dt);

	if (keyTaps['q'] || keyTaps['Q'])
		Mix_PlayChannel( 3, gSkyDive, -1 );

	/*--------Walk-Foward-----------------------------------------------------------*/
	if(keyStates['w'] || keyStates['W'])
	{

		camPos.x+=camLook.x*(crouchFactor*camSpeed*5.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*5.0f*dt);
	}

	/*---------sprint---------------------------------------------------------------*/
	if(keyStates['r'] || keyStates['R'])
	{
		camPos.x+=camLook.x*(crouchFactor*camSpeed*5.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*5.0f*dt);
	}

	/*--------Move-Backwards--------------------------------------------------------*/
	if(keyStates['s'] || keyStates['S'])
	{
		camPos.x-=camLook.x*(crouchFactor*camSpeed*5.0f*dt);
		//y-=ly*speed;  //comment out to walk backwards on plane
		camPos.z-=camLook.z*(crouchFactor*camSpeed*5.0f*dt);
	}

	/*--------Move-Left-------------------------------------------------------------*/
	if(keyStates['a'] || keyStates['A'])
	{
		vecx = camLook.z;
		vecz = -camLook.x;
		camPos.x+= vecx*(crouchFactor*camSpeed*5.0f*dt);
		camPos.z+= vecz*(crouchFactor*camSpeed*5.0f*dt);
	}

	/*--------Move-Right------------------------------------------------------------*/
	if(keyStates['d'] || keyStates['D'])
	{
		vecx = camLook.z;
		vecz = -camLook.x;
		camPos.x-= vecx*(crouchFactor*camSpeed*5.0f*dt);
		camPos.z-= vecz*(crouchFactor*camSpeed*5.0f*dt);		
	}

	/*---------crouch---------------------------------------------------------------*/
	if(keyTaps['c'] || keyTaps['C'])
	{
		if(!isCrouched)
		{
			addCrouch = -10.0f;
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
		camPos.y += (crouchFactor*camSpeed*5.0f*dt);

	if(keyStates['Z'] || keyStates['z'])
		camPos.y -= (crouchFactor*camSpeed*5.0f*dt);

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

		int tmpHeight = currentTerrain->getSample(camPos.x, camPos.z);
		if(camPos.y <= tmpHeight && currentTerrain->isInBounds(camPos.x, camPos.z))
		{
			camPos.y = tmpHeight;
			inJump = false;
		}
		else
		{
			if(camPos.y <= -5000.0f)
			{
				camPos = respawnPos;
				inJump = false;
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
		if(myPlayer->isAlive())
		{
			Vec3 tmp(0,0,0);
			camPos = tmp;
		}
		else
		{
			camPos= respawnPos;
			myPlayer->setAlive();
		}
	}
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
		//iterate through list of portals
		refresh = false;
		for(it = portalList.begin(); it!=portalList.end(); ++it)
		{
			//check if portal has expired. if so remove from list.
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

			//wait time of 2 seconds between portal jumps.
			if(!readyToWarp)
			{
				//last warp time + timer of 2 seconds.
				//if current time greater than endWarp ready to warp is true
				auto endWarp = lastWarp + chrono::milliseconds(2000);
				auto tmpnow = chrono::steady_clock::now();

				if(endWarp < tmpnow)
					readyToWarp = true;
			}

			//check if near portal and portal is open
			auto nowish = chrono::steady_clock::now();
			if((r.length()<100.0f) && readyToWarp == true && (*it)->Open() < nowish)
			{
					//update map
					inFirstWorld = !inFirstWorld;
					if(inFirstWorld)
						currentTerrain = &terrain;
					else
						currentTerrain = &terrain1;

				readyToWarp = false;
				lastWarp = chrono::steady_clock::now();

				return;
			}
		}
	}	
}
void updateEnemies()
{
	for(auto it = enemyList.begin(); it!=enemyList.end(); ++it)
	{
		if((*it)->target->isAlive() && (*it)->isAlive())
			(*it)->persue(dt);
	}
}
void updateBullets()
{
	/*first while checks if bullet is still live(before its end time), if not, remove from list*/
	bool refresh = true;
	list<Bullet*>::iterator it;
	auto now = chrono::steady_clock::now();
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
	/*iterate throught each bullet and check collision with position of each enemy*/
	list<Player*>::iterator it2;
	Vec3 r;
	bool fresh = true;
	while(fresh)
	{
		//set fresh to false to end while. iterate through each bullet.
		fresh = false;
		for(it = bulletList.begin(); it!=bulletList.end(); ++it)
		{
			//iterate through all enemies. tmp is bullet pos.
			Vec3 tmp = (*it)->returnPos();
			for(it2 = enemyList.begin(); it2!=enemyList.end(); ++it2)
			{
				//check for collision between bullet and enemy position.
				r = (*it2)->returnCollisionVec()-tmp;
				if(r.length() < (*it)->getDamageRadius())
				{
					//(*it2)->setDead();
					enemyList.erase(it2);
					numOfZombies--;
					fresh = true;
					/*bulletList.erase(it);
					delete *it;*/
				}
			}
		}
	}
}
void updateWeapons()
{
	/*----updates player inventory when player-WeaponPickup collision occurs------*/
	currentWeapon = NULL;
	bool refresh = true;
	while(refresh)
	{	
		//set refresh to false to end while. 
		refresh = false;
		for(auto i = weaponList.begin(); i != weaponList.end(); i++)
		{
			//check if in range of weapon. 
			currentWeapon = *i;
			if(currentWeapon->isInRange(camPos, 20))
			{
				Weapon* tmp;
				tmp = currentWeapon;
				tmp->reposition();

				//add to inventory
				myWeapons.push_back(tmp);
				weaponList.erase(i);
				myWeapon = myWeapons.size() - 1;
				cout << ">" << currentWeapon->getName() << "  added to inventory." << endl;
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
void drawZombies()
{
	list<Player*>::iterator it;
	for(it = enemyList.begin(); it!=enemyList.end(); ++it)
	{
		if((*it)->isAlive())
			(*it)->draw();
	}
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
void updatePlayer()
{
	if(!myPlayer->isAlive() && !myPlayer->isDeadCam())
	{
		cout << "Player is dead. " << endl;
		myPlayer->setRot(90.0f,1.0f,0,0);
		myPlayer->setDeadCam(true);
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
	if(!enemyList.empty())
		updateEnemies();
}
void renderLists()
{
	renderPortals();
	renderBullets();
	drawPickups();
	drawZombies();
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

			glPushMatrix();
				stormtrooper->draw();
			glPopMatrix();

		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
}
void secondWorldDraw()
{
	terrain1.drawElements();
	/*glDisable(GL_CULL_FACE);

		glPushMatrix();
			nimbus->draw();
		glPopMatrix();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);*/
}
void renderScene(void)
{
	handleFunc(dt);
	updateLists();
	updateCamera();
	updatePlayer();
	drawBackground();

	/*----Draw objects in 3d scene--------------------------------------------------*/
	// Reset transformations
	glLoadIdentity();
	gluLookAt(camPos.x,camPos.y+addCrouch,camPos.z,camPos.x+camLook.x,camPos.y+addCrouch+camLook.y,camPos.z+camLook.z,0,1,0);

	ballOfLight();

	if(inFirstWorld)
		firstWorldDraw();
	else
		secondWorldDraw();


			glPushMatrix();
				myPlayer->draw();
			glPopMatrix();

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
    //load sound with SDL_mixer functions
    gSkyDive = Mix_LoadWAV("./SFX/Skydive.wav");
    shootSFX = Mix_LoadWAV("./SFX/portalBlast.wav");
    gWrecked = Mix_LoadWAV("./SFX/riggity.wav");
    gBlap = Mix_LoadWAV("./SFX/blap3.wav");
    gShowMe = Mix_LoadWAV("./SFX/showMeWhatYouGot.wav");
    gPistol = Mix_LoadWAV("./SFX/pistol.wav");
    gAssualt = Mix_LoadWAV("./SFX/ak.wav");
    gRocket = Mix_LoadWAV("./SFX/rocket.wav");
    Mix_PlayChannel( 3, gShowMe, 0);
}
void load()
{
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	//Load Texture images
	texture = SOIL_load_OGL_texture("./textures/showMeWhatYouGot.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	Portals = SOIL_load_OGL_texture("./textures/portal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	PortalsCenter = SOIL_load_OGL_texture("./textures/portalcenter.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	PortalsOuter = SOIL_load_OGL_texture("./textures/spiral2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	sun = SOIL_load_OGL_texture("./textures/Sun1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	//Load Terrains
	cout << ">Loading .raw terrains ... " << endl;
	terrain.loadFile("./heightMaps/wallTest.raw", imageSize);
	terrain1.loadFile("./heightMaps/gta.raw", imageSize);
	cout << ">Terrains loaded." << endl << endl;

	//assign heights using map heights indxed.
	currentTerrain = &terrain;
	portalGunPos.y = currentTerrain->getSample(camPos.x+200, camPos.z+200);
	pistolPos.y = portalGunPos.y;
	assaultPos.y = portalGunPos.y;
	rocketPos.y = portalGunPos.y;

	//set initial camheight y
	camPos.y = currentTerrain->getSample(camPos.x, camPos.z) + 10.0f;

	Vec3 tmpPos (1.0f, -0.27f, -2.9f);
	Vec3 tmpPos2(0.7f, -0.50f, -2.9f);
	Vec3 tmpPos3(1.0f, -0.50f,  0.0f);
	Vec3 tmpPos4(0.0f, -0.35f,  0.0f);
	stormPos.y = currentTerrain->getSample(stormPos.x, stormPos.z);
	playerPos.x = camPos.x; playerPos.z = camPos.z;
	
	//Load Object files
	cout << ">Loading Objects ..." << endl;
	building = new Obj("./data/City/newCity.obj", "city #1");
	//nimbus = new Obj("./data/Sirus-City/sirus.obj", "city #2");
	wPortal = new Weapon("./data/Portal-Gun/portalgun_v3.obj", "portalGun", portalGunPos, tmpPos2, 6, 1, 180, 0, 1, 0, NULL);
	wPistol = new Weapon("./data/newGun/newgun.obj", "pistol", pistolPos, tmpPos, 8, 1, 90, 0, 1, 0, "./data/newGun/handgun_Fire.png");
	assault = new Weapon("./data/AK47/AK47.obj", "auto", assaultPos, tmpPos4, 8, 1, 180, 0, 1, 0, NULL);
	rocket = new Weapon("./data/rpg/rpg.obj", "rocket", rocketPos, tmpPos3, 20, 10, 180, 0, 1, 0, "");
	rock = new Item("./data/rpg/rocket.obj", "rock", rocketPos);
	stormtrooper = new Item("./data/stormtrooper/Stormtrooper.obj", "person", stormPos);
	myPlayer = new Player("./data/James_Bond/james_bond.obj", "player", playerPos, 13.4f, camLook);//26.5f);
	cout << ">All objects constructed." << endl;


	/*----Create Enemies------------------------------------------------------------*/
	Player* p;
	string name;
	//randomize x and z position, and sample y height of map based on random (x,0,z);
	//construct 25 zombie players, set target for zombies as myPlayer, add to enemyList
	for(int i= 0; i<52; ++i)
	{
		name = "zombie" + string(1, '0' + i);
		Vec3 tmp;
		tmp.x = (float)(rand())/(RAND_MAX)*((float)(currentTerrain->returnLength())*currentTerrain->scale());
		tmp.z = (float)(rand())/(RAND_MAX)*((float)(currentTerrain->returnLength())*currentTerrain->scale());
		tmp.y = currentTerrain->getSample(tmp);
		p = new Player("./data/Zombie/zombie_fast.obj", name.c_str(), tmp, 13.4f, camLook);
		p->setScale(12);
		p->setGrunt("./SFX/zombieGrunt.wav");
		p->setTarget(myPlayer);
		p->setBodyCenter(13.4f);
		p->setTerrain(currentTerrain);
		enemyList.push_back(p);
	}
	cout << ">All enemies constructed." << endl;

	/*----End of Load Object files--------------------------------------------------*/

	/*----create Lists--------------------------------------------------------------*/
	weaponList.push_back(wPistol);
	weaponList.push_back(wPortal);
	weaponList.push_back(assault);
	weaponList.push_back(rocket);
	cout << ">weaponList constructed." << endl;
	objList.push_back(stormtrooper);
	cout << ">objectList constructed." << endl;
	/*----end of lists--------------------------------------------------------------*/

	/*----set initial positions-----------------------------------------------------*/
	stormtrooper->setScale(8);
	myPlayer->setScale(8);

	building->setPos(7000.0f, currentTerrain->getSample(7000,portalGunPos.z+10000), portalGunPos.z+10000);
	building->setScale(4);

	/*nimbus->setPos(10244,288,8020);//nimbus->setPos(4000.0f, terrain1.getSample(4000,portalGunPos.z+5000), portalGunPos.z+5000);
	nimbus->setScale(4);*/
	/*------------------------------------------------------------------------------*/

    //cout << ">Setting weapon audio ..."<< endl;
    wPistol->pickupSFX = gBlap;
    wPortal->pickupSFX = gWrecked;
    rocket->pickupSFX = gRocket;

    //cout << ">Weapon audio enabled." <<endl;


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