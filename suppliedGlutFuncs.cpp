#include "suppliedGlutFuncs.h"

extern list<Portal*> portalList;
extern list<Bullet*> bulletList;
extern vector<Weapon*> myWeapons;
extern Mix_Chunk *shootSFX, *gPistol;
extern float pitch, heading, boost, aspectRatio, perspective;
extern int centerX, centerY, fire;
extern bool keyStates[256], keyTaps[256], inJump;
extern void* font;
extern float FPS, speed;
extern Vec3 camPos, camLook;
extern bool hasPortalGun, zoom, holdingPortalGun, hasPistol, holdingPistol, shoot;
extern void drawFire();
extern Weapon* nextWeapon, prevWeapon;
vector<Weapon*>::iterator it;
extern int myWeapon;
extern Terrain* currentTerrain;
extern Player* myPlayer;
extern Obj* nimbus;
extern int numOfZombies;
//extern chrono::steady_clock::time_point shootStart;

float x= camPos.x;
float y= camPos.y; 
float z= camPos.z;
float lx= camLook.x;
float ly= camLook.y;
float lz= camLook.z;

void initLight();
void initFog();

void initGL(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1024,768);
	glutCreateWindow("\t\t\t\t\t\tTEST MODE");
	glutFullScreen();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(pressNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(pressSpecialKeys);
	glutSpecialUpFunc(releaseSpecialKeys);

	// mouse movement input functions
	glutMouseFunc(mouseButton);
	glutPassiveMotionFunc(mouseMove);

	glutMotionFunc(mouseMove);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	initFog();
	initLight();

	//initialize bool arrays
	memset(keyStates, 0, 256*sizeof(bool));
	memset(keyTaps, 0, 256*sizeof(bool));

	// enter GLUT event processing cycle
	glutSetCursor(GLUT_CURSOR_NONE);
	centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;

	cout << ">OpenGL initialized." << endl;
}

void initFog()
{
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	float fogColor[] = {0.5, 0.5, 0.5, 1.0};
	float fogDensity= 0.01f;
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, fogDensity);
	glHint(GL_FOG_HINT, GL_NICEST);

	glFogf(GL_FOG_START, 2000.0f);
	glFogf(GL_FOG_END, 40000.0f);
}

void initLight()
{
	const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 1.0f }; 

	const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat mat_specular[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat high_shininess[] = { 100.0f };

	glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); 

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

/*    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001f);*/
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void mouseMove(int x, int y)
{
	if (!(x == centerX && y == centerY))
	{
		int dx = x - centerX;
		int dy = y - centerY;
	
		heading -= static_cast<float>(dx)*0.08f;
		pitch -= static_cast<float>(dy)*0.08f;

		if(pitch>85)
			pitch=85;
		else
		{
			if(pitch<-85)
				pitch = -85;
		}
		Vec3 newlook(0,0,-1);
		Vec3 newpos;

		// rotate around the X axis by 'pitch' degrees
		newpos.x = newlook.x;
		newpos.y = newlook.y * cosf(pitch * PI_OVER_180) - newlook.z * sinf(pitch * PI_OVER_180);
		newpos.z = newlook.y * sinf(pitch * PI_OVER_180) + newlook.z * cosf(pitch * PI_OVER_180);
		
		// rotate resulting vector around Y axis by 'heading' degrees,
		// then store into camLook.x,Y,Z
		camLook.x = newpos.x * cos(heading * PI_OVER_180) + newpos.z * sin(heading * PI_OVER_180);
		camLook.y = newpos.y;
		camLook.z = -newpos.x * sin(heading * PI_OVER_180) + newpos.z * cos(heading * PI_OVER_180);
		glutWarpPointer(centerX, centerY);

	}
}

void mouseButton(int button, int state, int x, int y) 
{
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{	
		if(!myWeapons.empty())
		{
			if(myWeapons[myWeapon]->getName() == "portalGun")
			{
				//myWeapons[myWeapon]->setShootTrue();
				Mix_PlayChannel( -1, shootSFX, 0 );
				Portal* warp = new Portal(camPos, camLook, chrono::steady_clock::now());
				portalList.push_back(warp);
			}
			if(myWeapons[myWeapon]->getName() == "pistol")
			{
				myWeapons[myWeapon]->setShootTrue();
				Mix_PlayChannel(-1, gPistol, 0);
				Bullet* ball = new Bullet(1.0f, 50.0f, camLook, camPos, chrono::steady_clock::now());
				bulletList.push_back(ball);
			}
			if(myWeapons[myWeapon]->getName() == "auto")
			{
				myWeapons[myWeapon]->setShootTrue();
				Mix_PlayChannel(-1, gPistol, 0);
				Bullet* ball = new Bullet(1.0f, 50.0f, camLook, camPos, chrono::steady_clock::now());
				bulletList.push_back(ball);
			}
		}
		else
		{}
	}

	if(button == GLUT_RIGHT_BUTTON)
	{
		if(perspective < 45.0f && state == GLUT_DOWN)
		{
			perspective= 45.0f;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(perspective, aspectRatio, 0.1f, 40000.0f);
			glMatrixMode(GL_MODELVIEW);
		}
		else if(state == GLUT_DOWN && zoom == false)
			zoom = true;
		else if(state == GLUT_UP)
			zoom = false;
	}

	if(button == 3 || button == 4)
	{
		if(state == GLUT_UP)
			return;
		else
		{
			if (button == 3)
			{
				if(!myWeapons.empty())
				{
					cout << ">Scroll UP." << endl;
					if(myWeapon== 0)
						{
							myWeapon = myWeapons.size() -1;
						}
					else
						myWeapon--;
				}
			}
			else if(button == 4)
			{
				if(!myWeapons.empty())
				{
					cout << ">Scroll DOWN." << endl;
					if(myWeapon == ((int)(myWeapons.size() -1)))
					{
						myWeapon = 0;
					}
					else
						myWeapon++;
				}
			}
		}
	}
}

void pressNormalKeys(unsigned char key, int xx, int yy) 
{
	if(key == 27)
	{	cout<< "> ESC:\tExiting Program." << endl;
		exit(EXIT_FAILURE);
	}

	keyStates[key]=true;
	keyTaps[key]=true;
}

void releaseNormalKeys(unsigned char key, int xx, int yy)
{
	/*	if(key == keyStates['q'] || key == keyStates['Q'])
		inJump = true;*/
	keyStates[key]=false;
	keyTaps[key]=false;

	if(key == 'q' || key == 'Q')
	{
		Mix_HaltChannel(3);
	}
}

void pressSpecialKeys(int key, int xx, int yy) 
{
	if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
	{

	}
} 

void releaseSpecialKeys(int key, int x, int y) 
{
	if (!(glutGetModifiers() & GLUT_ACTIVE_SHIFT))
	{

	}
} 

void renderBitmapString(float d, float e, float f, void *font, char *string)
{
    char *c;
    glRasterPos3f(d,e,f);
 
    for (c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

void renderText2D(float a, float b, void* font, char* string)
{
    glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
            glPushMatrix();
                glLoadIdentity();
                glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, 1.0, 5000.0);
                renderBitmapString(a, b, -1.0f, font, string);
            glPopMatrix();
        glMatrixMode(GL_MODELVIEW);    
    glPopMatrix();
}

void printScreenText()
{
	char msg[80];

    glColor3f(0.43f, 0, 0.03f);
 	glDisable(GL_LIGHTING);
    sprintf(msg, "cam pos: %.3f, %.3f, %.3f", camPos.x, camPos.y, camPos.z);
    renderText2D(5.0f, 20.0f, font, msg);

    sprintf(msg, "FOV: %.1f", perspective);
    renderText2D(5.0f, 40.0f, font, msg);

    sprintf(msg, "look vector: %.3f, %.3f, %.3f", camLook.x, camLook.y, camLook.z);
    renderText2D(5.0f, 60.0f, font, msg);

    sprintf(msg, "Speed: %.3f", speed);
    renderText2D(5.0f, 80.0f, font, msg);

    sprintf(msg, "FPS: %.3f", FPS);
    renderText2D(5.0f, 100.0f, font, msg);  

    sprintf(msg, "Movement:");
    renderText2D(5.0f, glutGet(GLUT_WINDOW_HEIGHT) - 100.0f, font, msg);

    sprintf(msg, "Space - JUMP");
    renderText2D(5.0f, glutGet(GLUT_WINDOW_HEIGHT) - 80.0f, font, msg);  

    sprintf(msg, "W - FORWARD");
    renderText2D(145.0f, glutGet(GLUT_WINDOW_HEIGHT) - 80.0f, font, msg);      

    sprintf(msg, "S - BACKWARD");
    renderText2D(145.0f, glutGet(GLUT_WINDOW_HEIGHT) - 60.0f, font, msg);  

    sprintf(msg, "A - LEFT");
    renderText2D(145.0f, glutGet(GLUT_WINDOW_HEIGHT) - 40.0f, font, msg);  

    sprintf(msg, "D - RIGHT");
    renderText2D(145.0f, glutGet(GLUT_WINDOW_HEIGHT) - 20.0f, font, msg);  

    sprintf(msg, "Q - FLY");
    renderText2D(145.0f, glutGet(GLUT_WINDOW_HEIGHT) - 100.0f, font, msg);  

    sprintf(msg, "LEFT CLICK - SHOOT");
    renderText2D( glutGet(GLUT_WINDOW_WIDTH) - 240.0f,glutGet(GLUT_WINDOW_HEIGHT) - 100.0f,font, msg); 

    sprintf(msg, "RIGHT CLICK - ZOOM");
    renderText2D( glutGet(GLUT_WINDOW_WIDTH) - 240.0f, glutGet(GLUT_WINDOW_HEIGHT) - 80.0f,font, msg); 

    sprintf(msg, "In Bounds: %d", currentTerrain->isInBounds(camPos));
    renderText2D(700.0f, 20.0f, font, msg);

    sprintf(msg, "current height:\t%.3f", currentTerrain->getSample(camPos));
    renderText2D(700.0f, 40.0f, font, msg);

    sprintf(msg, "myPlayer position:\t%.3f, %.3f, %.3f", myPlayer->pos.x, myPlayer->pos.y, myPlayer->pos.z);
    renderText2D(700.0f, 60.0f, font, msg);

	sprintf(msg, "myPlayer deaths:\t%d", myPlayer->deaths);
	renderText2D(700.0f, 80.0f, font, msg);

	if(!myPlayer->isAlive())
	{
		sprintf(msg, "\tYou DIED. Press TAB to respawn.");
		renderText2D(400.0f, 600.0f, font, msg);
	}

    if(nimbus != NULL)
	{
		sprintf(msg, "nimbasa pos:  %.3f, %.3f, %.3f", nimbus->pos.x, nimbus->pos.y, nimbus->pos.z);
	    renderText2D(700.0f, 80.0f, font, msg);
	} 

    sprintf(msg, "Zombies:\t%d", numOfZombies);
    renderText2D(450, 20.0f, font, msg);

    sprintf(msg, "myPlayer health:\t%.3f,", myPlayer->getHealth());
    renderText2D(450, 40.0f, font, msg);

    glEnable(GL_LIGHTING);
}

void changeSize(int w, int h) 
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio =  w * 1.0 / h;
	aspectRatio = ratio;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(perspective, aspectRatio, 0.1f, 40000.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}
