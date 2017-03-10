#include "suppliedGlutFuncs.h"
/*
#define TRUE 1
#define FALSE 0*/

extern list<Portal*> portalList;
extern list<Bullet*> bulletList;
extern Mix_Chunk *shootSFX, *gPistol;
extern float pitch, heading, boost, aspectRatio, perspective;
extern int centerX, centerY, fire;
extern bool keyStates[256], keyTaps[256], inJump;
extern void* font;
extern float FPS, speed;
extern Vec3 camPos, camLook;
extern bool hasPortalGun, zoom, holdingPortalGun, hasPistol, holdingPistol, shoot;
extern void drawFire();
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

	// here are the two new functions
	glutMouseFunc(mouseButton);
	glutPassiveMotionFunc(mouseMove);

	glutMotionFunc(mouseMove);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	initFog();
	initLight();

	memset(keyStates, 0, 256*sizeof(bool));
	memset(keyTaps, 0, 256*sizeof(bool));

	// enter GLUT event processing cycle
	glutSetCursor(GLUT_CURSOR_NONE);
	centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
}

void initFog()
{
	/*----------Fog Code--------*/
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
	/*---------Light Code--------*/
	const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 1.0f }; 

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
		if(hasPortalGun && !holdingPistol)
		{
			Mix_PlayChannel( -1, shootSFX, 0 );
			Portal* warp = new Portal(camPos, camLook, chrono::steady_clock::now());
			portalList.push_back(warp);		
		}
		if(holdingPistol == TRUE && hasPistol == TRUE)
		{
			Mix_PlayChannel(-1, gPistol, 0);
			shoot = TRUE;
			Bullet* ball = new Bullet(1.0f, 50.0f, camLook, camPos, chrono::steady_clock::now());
			bulletList.push_back(ball);
		}
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
		else if(state == GLUT_DOWN && zoom == FALSE)
			zoom = TRUE;
		else if(state == GLUT_UP)
			zoom = FALSE;
	}

	if(button == 3 || button == 4)
	{
		if(state == GLUT_UP)
			return;
		else
		{
			if (button == 3)
			{
				//cout << "scroll UP." << endl;
				if(hasPortalGun)
					holdingPortalGun = !holdingPortalGun;
				if(hasPistol)
					holdingPistol = !holdingPistol;
			}
			else if(button == 4)
			{
				//cout << "scroll DOWN." << endl;
				if(hasPortalGun)
					holdingPortalGun = !holdingPortalGun;
				if(hasPistol)
				holdingPistol = !holdingPistol;
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
		inJump = TRUE;*/
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
	char msg2[80];
	char msg3[80];
	char msg4[80];
	char msg5[80];
    glColor3f(0, 0.6f, 0);
 	glDisable(GL_LIGHTING);
    sprintf(msg, "cam pos: %.3f, %.3f, %.3f", camPos.x, camPos.y, camPos.z);
    renderText2D(5.0f, 20.0f, font, msg);

    sprintf(msg2, "FOV: %.1f", perspective);
    renderText2D(5.0f, 40.0f, font, msg2);

    sprintf(msg3, "look vector: %.3f, %.3f, %.3f", camLook.x, camLook.y, camLook.z);
    renderText2D(5.0f, 60.0f, font, msg3);

    sprintf(msg4, "Speed: %.3f", speed);
    renderText2D(5.0f, 80.0f, font, msg4);

    sprintf(msg5, "FPS: %.3f", FPS);
    renderText2D(5.0f, 100.0f, font, msg5);    
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
