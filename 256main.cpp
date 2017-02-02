#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <stdio.h>
#include <string>
#include <chrono>
#include "DrawFunc.h"
#include "suppliedGlutFuncs.h"
#include "Terrain.h"
#include "Vec3.h"
#include "Portal.h"
#include "Obj.h"
/*---------------*/
#include <SOIL.h>
#include <SDL.h>
#undef main
#include <SDL_mixer.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

#define GRAVITY -981.0f//-98.10f
#define imageSize 256

/*-----Points-in-Time-----*/
chrono::steady_clock::time_point currentTime, lastTime, jumpStart, portalPassTime;
chrono::steady_clock::time_point lastWarp;

/*-------Objects----------*/
Obj* stormtrooper = NULL;
Obj* building = NULL;
Obj* pistol = NULL;
Obj* pist2 = NULL;
Obj* tree = NULL;

/*------Bool-Inputs-------*/
bool readyToWarp = TRUE;
bool keyStates[256], keyTaps[256];
bool isCrouched = FALSE;
bool inJump = FALSE;
bool inFirstWorld = TRUE;
bool firingWeapon = FALSE;
bool hasPortalGun = FALSE;
bool hasPistol = FALSE;
bool zoom = FALSE;
bool wireFrame = FALSE;
bool inBounds = TRUE;

/*------View-Angles-------*/
float heading = 0.0f;
float pitch = 0.0f;
float perspective = 45.0f;
float aspectRatio = 1024.0f/768.0f;	

/*------Frame-Data--------*/
float FPS = 0.0f;
float fpsUpdateTime = 0.0f;
int frameCount = 0;

/*--Camera Position and Look Vectors--*/
Vec3 camLook(0.0f,0.0f,-1.0f);
//Vec3 camPos(105050.0f, 12000.0f,9000.0f);
Vec3 camPos(50058.21f, 12144.12f, 95300.75f);//camPos(5005.821f, 1214.412f, 9530.075f);//camPos(1725.0f,5.0f,4308.0f);
Vec3 portalGunPos(camPos.x, camPos.y, camPos.z-1000);
Vec3 pistolPos(camPos.x+1000, 14825, camPos.z-1000);
Vec3 respawnPos(50058.21f, 121441.20f, 95300.75f);
Vec3 lightPos(50058.21f, 16830.0f, 90000.0f);

GLfloat light_position[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };


/*------Velocities--------*/
float dt;
float speed, flySpeed=0.15;
float updateSpeedTime = 0.0f;
float camSpeed = 0.04f;
float addCrouch = 0.0f;
float crouchFactor = 1.0f;
float jumpVelocity = 1500.0f;//200.0f;
float initialJumpHeight = 0.0f;
Vec3 prevPos=camPos;

/*------Texture-Data------*/
GLUquadricObj* quadric;
int centerX, centerY, texture;
void* font = GLUT_BITMAP_HELVETICA_18;
bool loadHiRes;

/*------Portals-----------*/
list<Portal*> portalList;
int PortalsCenter, PortalsOuter, Portals;
float iconSpin = 0;

/*------Sounds------------*/
Mix_Music *gMusic = NULL;
Mix_Chunk *shootSFX = NULL;
Mix_Chunk *gSkyDive = NULL;
Mix_Chunk *gWrecked = NULL;
Mix_Chunk *gShowMe = NULL;

/*------Terrain-Data------*/
float* currentMap;
float* mapHeights;
float* mapHeights1;
float scale;
Terrain terrain(imageSize,imageSize);
Terrain terrain1(imageSize,imageSize);
Terrain terrain2(imageSize,imageSize);
Terrain terrain3(imageSize,imageSize);

int xt = 0; int zt = 0; int index = 0;

void handleFunc(float dt)
{
	float vecx, vecy, vecz;

	if(keyTaps['1'])
		inFirstWorld = !inFirstWorld;

	/*--Zoom--*/
	if(zoom)
	{
		if(perspective <= 2.0f)
		{
			perspective = 5.0f;
			zoom = FALSE;
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

	/*--------change perspective---------*/
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

	/*---------Fly-Foward---------*/
	if(keyStates['q'] || keyStates['Q'])
		camPos+=camLook*(flySpeed*13.5f*dt);

	if (keyTaps['q'] || keyTaps['Q'])
		Mix_PlayChannel( 3, gSkyDive, -1 );

	/*--------Move-Foward---------*/
	if(keyStates['w'] || keyStates['W'])
	{

		camPos.x+=camLook.x*(crouchFactor*camSpeed*20.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*20.0f*dt);
	}

	/*---------sprint-------------*/
	if(keyStates['r'] || keyStates['R'])
	{
		camPos.x+=camLook.x*(crouchFactor*camSpeed*20.0f*dt);
		//camPos.y+=camLook.y;	//comment out to walk forward on plane
		camPos.z+=camLook.z*(crouchFactor*camSpeed*20.0f*dt);
	}

	/*--------Move-Backwards------*/
	if(keyStates['s'] || keyStates['S'])
	{
		camPos.x-=camLook.x*(crouchFactor*camSpeed*20.0f*dt);
		//y-=ly*speed;  //comment out to walk backwards on plane
		camPos.z-=camLook.z*(crouchFactor*camSpeed*20.0f*dt);
	}

	/*--------Move-Left----------*/
	if(keyStates['a'] || keyStates['A'])
	{
		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x+= vecx*(crouchFactor*camSpeed*20.0f*dt);
		camPos.z+= vecz*(crouchFactor*camSpeed*20.0f*dt);
	}

	/*--------Move-Right----------*/
	if(keyStates['d'] || keyStates['D'])
	{
		vecx = camLook.z;
		vecy = camLook.y;
		vecz = -camLook.x;
		camPos.x-= vecx*(crouchFactor*camSpeed*20.0f*dt);
		camPos.z-= vecz*(crouchFactor*camSpeed*20.0f*dt);		
	}

	/*---------crouch-------------*/
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
	if(keyStates['x'] || keyStates['X'])
		camPos.y += (crouchFactor*camSpeed*13.0f*dt);

	if(keyStates['Z'] || keyStates['z'])
		camPos.y -= (crouchFactor*camSpeed*13.0f*dt);

	/*----------jump--------------*/
	if(keyStates[' '])
	{
		inJump = TRUE;
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
			inBounds = FALSE;

		if(camPos.y <= (scale * currentMap[index] * 1500.0f) && inBounds)
		{
			camPos.y = scale * currentMap[index] * 1500.0f;
			inJump = FALSE;
		}
		else if(!inBounds)
		{
			if(camPos.y <= -5000.0f)
			{
				camPos = respawnPos;
				inJump = TRUE;
				inBounds = TRUE;
			}
		}
	}

	/*-----Toggle wireframe-----*/
	if(keyTaps['t'] || keyTaps['T'])
	{
		wireFrame = !wireFrame;

		if(wireFrame)
			glPolygonMode(GL_FRONT, GL_LINE);
		else
			glPolygonMode(GL_FRONT,GL_FILL);
	}
	if(keyTaps['\t'])
	{
		Vec3 tmp(0,0,0);
		camPos = tmp;
	}

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

void updateCamHeight()
{
	zt = static_cast<int>(camPos.z)/(64*scale);
	xt = static_cast<int>(camPos.x)/(64*scale); 
		if(zt<0 || xt<0)
		return;
	index = zt * imageSize + xt;
	if(camPos.y < (currentMap[index] * 1500.0f - 2000.0f))
		return;
	else
		camPos.y = scale * currentMap[index] * 1500.0f;
}
bool floorDistance()
{
	zt = static_cast<int>(camPos.z)/(64*scale);
	xt = static_cast<int>(camPos.x)/(64*scale); 
	if(zt<0 || xt<0)
		return FALSE;
	index = zt * imageSize + xt;
	if(camPos.y > (scale * currentMap[index] * 1500.0f - (25.0f )))// - 150.0f))
		return FALSE;
	else if(camPos.y < (currentMap[index] * 1500.0f - (scale * 300.0f)))
		return FALSE;
	else
		return TRUE;
}

void mapItem()
{

	//initial position of mapItem
	int tmpzt = static_cast<int>(camPos.z - 1000)/(64*scale);
	int tmpxt = static_cast<int>(camPos.x)/(64*scale); 
	index = tmpzt * imageSize + tmpxt;
	portalGunPos.y = scale * currentMap[index] * 1500.0f;



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
				auto endWarp = lastWarp + chrono::milliseconds(3000);
				auto tmpnow = chrono::steady_clock::now();

				if(endWarp<tmpnow)
					readyToWarp = TRUE;
			}

			auto nowish = chrono::steady_clock::now();
			//within bounds
			if((r.length()<200.0f) && readyToWarp == TRUE
				&& (*it)->Open() < nowish)
			{
					inFirstWorld = !inFirstWorld;
					if(inFirstWorld)
					{
						currentMap = mapHeights;
					}
					else
					{
						currentMap = mapHeights1;
					}	

				readyToWarp = FALSE;
				lastWarp = chrono::steady_clock::now();

				return;
			}
		}
	}	

}

void renderPortals()
{
	list<Portal*>::iterator it;
	for(it = portalList.begin(); it!=portalList.end(); ++it)
	{
		(*it)->openPortal();
	}
}



void renderScene(void)
{
	currentTime = chrono::steady_clock::now();
	dt = chrono::duration_cast<chrono::duration<float, milli>>(currentTime-lastTime).count();
    lastTime = currentTime;

    fpsUpdateTime += dt;
	

	// Clear Color and Depth Buffers
	glClearColor(0.5f,0.5f,0.5f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(camPos.x >= 0 && camPos.z >= 0)
	{
		if(floorDistance()) 
			updateCamHeight();
	}

	updatePortals();
	handleFunc(dt);

	speed = camPos.distance(prevPos) * dt / 1000.0f;

	// Reset transformations
	glLoadIdentity();

		/*--------background---------*/
		gluLookAt(0,0,0,camLook.x,camLook.y,camLook.z,0,1,0);
			//glCullFace(GL_FRONT);
			glDisable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);

				glColor3f(1,1,1);
				glTranslatef(500.0f,500.0f,-1500.0f);
				glRotatef(90, 1,0,0);
				texturePlane(texture, 496.0f, 692.0f);

			glEnable(GL_CULL_FACE);
			glDisable(GL_TEXTURE_2D);
			glCullFace(GL_BACK);
			glDepthMask(GL_TRUE);

	glLoadIdentity();
	gluLookAt(camPos.x,camPos.y+addCrouch,camPos.z,camPos.x+camLook.x,camPos.y+addCrouch+camLook.y,camPos.z+camLook.z,0,1,0);

	//const GLfloat light_position[] = { 0.0f, 5.0f, 0.0f, 1.0f };

	GLfloat light_position[] = { lightPos.x, lightPos.y, lightPos.z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glPushMatrix(); 
		//glTranslatef(50058.21f, 16825.0f, 90000.0f);
		glTranslatef(lightPos.x,lightPos.y+200,lightPos.z);
		glColor4f(1,1,1,1);
		glutSolidSphere(200, 15, 15);
	glPopMatrix();

	if(inFirstWorld)
	{
		glEnable(GL_FOG);
		terrain.draw(wireFrame);
		glPushMatrix();
			glTranslatef(portalGunPos.x, 14825, portalGunPos.z-5000);
			
			//check if in range of floating pistol
			if(camPos.distance(pistolPos) < 200.0f && (hasPistol == FALSE))
			{
				hasPistol = TRUE;
			}

			if(hasPistol == FALSE)
			{
				glPushMatrix();
				glTranslatef(1000,0,5000);
				glScalef(85.0f,85.0f,85.0f);
				glRotatef(iconSpin, 0,1,0);
				pistol->draw();
				glPopMatrix();				
			}

			glPushMatrix();
			glScalef(85.0f, 85.0f, 85.0f);
			stormtrooper->draw();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-200,0,0);
			//glTranslatef(50058.21f, 12144.12f, 95300.75f);
			glScalef(200,200,200);
			tree->draw();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(200.0f, 0, 0);
			glScalef(85.0f,85.0f,85.0f);
			pist2->draw();
			glTranslatef(2.0f,0.0f,0);
			pist2->draw();
			glPopMatrix();


			glTranslatef(0,-10000.0f, 0);
			glScalef(40.0f, 40.0f, 40.0f);
			building->draw();
	
		glPopMatrix();

		//glDisable(GL_FOG);
	}
	else
	{
		glEnable(GL_FOG);
		terrain1.draw(wireFrame);
	}

	glDisable(GL_CULL_FACE);


	renderPortals();



	//check if in range of floating icon
	if(camPos.distance(portalGunPos) < 200.0f && (hasPortalGun == FALSE))
	{
		hasPortalGun = true;
		Mix_PlayChannel( -1, gWrecked, 0 );
	}
	
	if(hasPortalGun == FALSE)
	{	
																	//Portal gun icon pickup
														//maps height index of item
		glTranslatef(portalGunPos.x, portalGunPos.y, portalGunPos.z);		//translate to item pos
		glScalef(700,700,700);											//scales larger
		iconSpin += (360.0f * dt / 1000.0f);						//spins 2pi per sec
		glRotatef(iconSpin,0,1,0);
		drawPortalGun(dt);
	}
	else
	{										//hasPortalGun
		glLoadIdentity();					//translate and rotate infront of camera
		glTranslatef(0.15f,-0.07f,-0.3f);	//to get an FPS look
		glRotatef(180, 0,1,0);
		drawPortalGun(dt);
		glCullFace(GL_BACK);		
	}

	if(hasPistol)
	{
		glLoadIdentity();
		glTranslatef(1.25f, 0.0f, -5.0f);
		glRotatef(90,0,1,0);
		glScalef(1.0f,1.0f,1.0f);
		pistol->draw();
		glPopMatrix();
	}


	memset(keyTaps,0,256*sizeof(bool));

	/*----Update Frames-------*/
	if (fpsUpdateTime > 100.0f)
	{
		FPS = static_cast<float>(frameCount) / fpsUpdateTime * 1000.0f;
		frameCount = 0;
		fpsUpdateTime = 0.0f;
	}

	/*----Update prevPos------*/
	prevPos = camPos;		

    printScreenText();
    frameCount++;
	glutSwapBuffers();
}

void initAudio()
{
	bool success;
	//Initialize SDL_audio
	if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    gMusic = Mix_LoadMUS("./SFX/SpaceMusic.mp3");
    //gMusic = Mix_LoadMUS( "./SFX/MaculateEsseks.mp3" );
    Mix_PlayMusic( gMusic, -1 );

    /*--------Sound FX---------*/
    gSkyDive = Mix_LoadWAV("./SFX/Skydive.wav");
    shootSFX = Mix_LoadWAV("./SFX/portalBlast.wav");
    gWrecked = Mix_LoadWAV("./SFX/riggity.wav");
    gShowMe = Mix_LoadWAV("./SFX/showMeWhatYouGot.wav");
    Mix_PlayChannel( 3, gShowMe, 0);

/*    if( shootSFX == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }*/
}
int main(int argc, char **argv)
{

	// initialize GLUT and create window
	initGL(argc, argv);
	initAudio();

	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);

	terrain.loadFile("./heightMaps/wall.raw", imageSize);
	terrain1.loadFile("./heightMaps/gta.raw", imageSize);

	mapHeights = new float[imageSize*imageSize];
	mapHeights = terrain.returnHeights();
	currentMap = mapHeights;
	scale = terrain.scale();

	/*---set initial camheight y-------*/
	int tmpx = static_cast<int>(camPos.z)/(64*scale);
	int tmpz = static_cast<int>(camPos.x)/(64*scale); 
	index = tmpz * imageSize + tmpx;
	camPos.y = scale * currentMap[index] * 1500.0f;

	stormtrooper = new Obj("./data/stormtrooper/Stormtrooper.obj");
	building = new Obj("./data/City/City.obj");
	pist2 = new Obj("./data/newgun.obj");
	pistol = new Obj("./data/newgun.obj");
	tree = new Obj("./data/Tree/Tree.obj");

	mapItem();

	mapHeights1 = new float[imageSize*imageSize];
	mapHeights1 = terrain1.returnHeights();

	texture = SOIL_load_OGL_texture("./textures/showMeWhatYouGot.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	Portals = SOIL_load_OGL_texture("./textures/portal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	PortalsCenter = SOIL_load_OGL_texture("./textures/portalcenter.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	PortalsOuter = SOIL_load_OGL_texture("./textures/spiral2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

	lastTime = chrono::steady_clock::now();
	glutMainLoop();
	glutSetCursor(GLUT_CURSOR_INHERIT);

	return 1;
}