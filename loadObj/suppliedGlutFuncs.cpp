#include "suppliedGlutFuncs.h"

extern float pitch, heading, aspectRatio, perspective;
extern int centerX, centerY;
extern bool keyStates[256], keyTaps[256];
//extern void* font;
extern float FPS;//, speed;
extern Vec3 camPos, camLook;

float x= camPos.x;
float y= camPos.y; 
float z= camPos.z;
float lx= camLook.x;
float ly= camLook.y;
float lz= camLook.z;


void setupLight();
void setupFog();
void reshape(int w, int h);
extern void display();
void initGL(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(800,450);
	glutInitWindowPosition(20,20);
	glutCreateWindow("ObjLoader");
	
	// register callbacks
	glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	glutIdleFunc(display);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(pressNormalKeys);
	glutSpecialFunc(pressSpecialKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
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


	setupFog();
	setupLight();

	memset(keyStates, 0, 256*sizeof(bool));
	memset(keyTaps, 0, 256*sizeof(bool));

	// enter GLUT event processing cycle
	glutSetCursor(GLUT_CURSOR_NONE);
	centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
}

void setupLight()
{
	/*---------Light Code--------*/
	const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 1.0f }; 

	const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
	const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
	const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat high_shininess[] = { 100.0f };

	glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); 

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); 

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void setupFog()
{
	/*----------Fog Code--------*/
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	float fogColor[] = {0.5, 0.5, 0.5, 1.0};
	float fogDensity= 0.01f;
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, fogDensity);
	glHint(GL_FOG_HINT, GL_NICEST);

	glFogf(GL_FOG_START, 200.0f);
	glFogf(GL_FOG_END, 1000.0f);
}

void mouseMove(int x, int y)
{
	if (!(x == centerX && y == centerY))
	{
		int dx = x - centerX;
		int dy = y - centerY;
	
		heading -= static_cast<float>(dx)*0.08f;
		pitch -= static_cast<float>(dy)*0.08f;

		Vec3 newLook(0,0,-1);
		Vec3 newPos;

		// rotate around the X axis by 'pitch' degrees
		newPos.x = newLook.x;
		newPos.y = newLook.y * cosf(pitch * PI_OVER_180) - newLook.z * sinf(pitch * PI_OVER_180);
		newPos.z = newLook.y * sinf(pitch * PI_OVER_180) + newLook.z * cosf(pitch * PI_OVER_180);
		
		// rotate resulting vector around Y axis by 'heading' degrees,
		// then store into camLook.x,Y,Z
		camLook.x = newPos.x * cos(heading * PI_OVER_180) + newPos.z * sin(heading * PI_OVER_180);
		camLook.y = newPos.y;
		camLook.z = -newPos.x * sin(heading * PI_OVER_180) + newPos.z * cos(heading * PI_OVER_180);
		glutWarpPointer(centerX, centerY);

	}
}

void mouseButton(int button, int state, int x, int y) 
{
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
	
	}

	if(button == GLUT_RIGHT_BUTTON)
	{
		if(perspective < 45.0f && state == GLUT_DOWN)
		{
			perspective= 45.0f;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(perspective, aspectRatio, 0.1f, 10000.0f);
			glMatrixMode(GL_MODELVIEW);
		}
	}

	if(button == 3 || button == 4)
	{
		if(state == GLUT_UP)
			return;
		else
		{
			if (button == 3)
				cout << "scroll UP." << endl;
			else if(button == 4)
				cout << "scroll DOWN." << endl;
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
	keyStates[key]=false;
	keyTaps[key]=false;
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
 
/*    sprintf_s(msg, 79, "cam pos: %.3f, %.3f, %.3f", camPos.x, camPos.y, camPos.z);
    renderText2D(5.0f, 20.0f, font, msg);

    sprintf_s(msg2, 79, "FOV: %.1f", perspective);
    renderText2D(5.0f, 40.0f, font, msg2);

    sprintf_s(msg3, 79, "look vector: %.3f, %.3f, %.3f", camLook.x, camLook.y, camLook.z);
    renderText2D(5.0f, 60.0f, font, msg3);

    sprintf_s(msg4, 79, "Speed: %.3f", speed);
    renderText2D(5.0f, 80.0f, font, msg4);

    sprintf_s(msg5, 79, "FPS: %.3f", FPS);
    renderText2D(5.0f, 100.0f, font, msg5);    */

}

void reshape(int w, int h) 
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
	gluPerspective(perspective, aspectRatio, 0.1f, 10000.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}
