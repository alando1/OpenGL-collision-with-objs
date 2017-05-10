#include "DrawFunc.h"
extern Vec3 campos, camlook;
extern chrono::steady_clock::time_point lastTime;

float portalAngle = 0;
float itemSpin = 0;

void drawgrid()
{
	glBegin(GL_LINES);
	float xx;
	for (int a = -200; a < 200; a++)
		{	
			xx= (float)(a+200)/(400.0f);
			glColor3f(1.0f, xx/2, a);
			glVertex3f(-200, 0, a);
			glVertex3f(200, 0, a);

			glColor3f(1.0f, xx/2, a);
			glVertex3f(a, 0, -200);
			glVertex3f(a, 0, 200);

			glColor3f(a, 1.0f, xx/2);
			glVertex3f(a, -200, 0);
			glVertex3f(a, 200, 0);

			glColor3f(a, 1.0f, xx/2);
			glVertex3f(-200, a, 0);
			glVertex3f(200, a, 0);

			glColor3f(xx/2, a, 1.0f);
			glVertex3f(0, a, -200.0f);
			glVertex3f(0, a, 200.0f);

			glColor3f(xx/2, a, 1.0f);
			glVertex3f(0, -200, a);
			glVertex3f(0, 200, a);
		}
	glEnd();
}

void otherGrid()
{
	glBegin(GL_LINES);
	glColor3f(0.250f, 0.325f, 0.75f);
	float w;
	for (int a = -2000; a < -1000; a++)
		{	
			w= (float)(a+2000)/1000.0f;
			glColor3f(1.0f, w/2, a);
			glVertex3f(-2000, -2000, a);
			glVertex3f(-1000, -2000, a);

			glColor3f(1.0f, w/2, a);
			glVertex3f(a, -2000, -2000);
			glVertex3f(a, -2000, -1000);

			glColor3f(a, 1.0f, w/2);
			glVertex3f(a, -2000, -2000);
			glVertex3f(a, -1000, -2000);

			glColor3f(a, 1.0f, w/2);
			glVertex3f(-2000, a, -2000);
			glVertex3f(-1000, a, -2000);

			glColor3f(w/2, a, 1.0f);
			glVertex3f(-2000, a, -2000.0f);
			glVertex3f(-2000, a, -1000.0f);

			glColor3f(w/2, a, 1.0f);
			glVertex3f(-2000, -2000, a);
			glVertex3f(-2000, -1000, a);
		}
	glEnd();
}

void drawFloor()
{
	glBegin(GL_QUADS);
		glVertex3f(-200.0f, 0.0f, -200.0f);
		glVertex3f(-200.0f, 0.0f,  200.0f);
		glVertex3f( 200.0f, 0.0f,  200.0f);
		glVertex3f( 200.0f, 0.0f, -200.0f);
	glEnd();
}

void texturePlane(int testTexture, float length, float width)
{
	length = length/2.0f;
	width = width/2.0f;

	/*-------test texturing----*/
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, testTexture);
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	float scale = 1.0f;
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-length, 0.0f, width);
		glTexCoord2f(scale, 0);
		glVertex3f( length, 0.0f, width);
		glTexCoord2f(scale, scale);
		glVertex3f( length, 0.0f, -width);
		glTexCoord2f(0, scale);
		glVertex3f(-length, 0.0f, -width);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void drawShed()
{
	glBegin(GL_QUADS);
		glColor4f(0.3f,0.0f,0.0f,0.9f);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1000);
	glEnd();

	glBegin(GL_QUAD_STRIP);
		glColor4f(0.3f,0.0f,0.0f,0.9f);
		glVertex3f(0,-100,0);
			glVertex3f(1000,-100,0);
		glVertex3f(0,500,0);
			glVertex3f(1000,500,0);
		glVertex3f(0,500,1000);
			glVertex3f(1000,500,1000);
		glVertex3f(0,-100,1000);
			glVertex3f(1000,-100,1000);
	glEnd();
}

void buildHouse()
{

}

void drawPortalGun(float dt)
{
	GLUquadricObj* quadric;
	quadric = gluNewQuadric();

	glPushMatrix();
	glScalef(0.008f,0.008f,0.008f);
	glBegin(GL_QUADS);
		glColor3f(0.8f,0.8f,0.8f);
		glVertex3f(0,0,0);
		glVertex3f(0,5,0);
		glVertex3f(0,5,20);
		glVertex3f(0,0,20);

		glNormal3f(-1,0,0);

		glVertex3f(10,0,0);
		glVertex3f(10,5,0);
		glVertex3f(10,5,20);
		glVertex3f(10,0,20);

		glNormal3f(1,0,0);

		glVertex3f(0,5,0);
		glVertex3f(10,5,0);
		glVertex3f(10,5,20);
		glVertex3f(0,5,20);

		glNormal3f(0,1,0);

		glVertex3f(0,0,0);
		glVertex3f(10,0,0);
		glVertex3f(10,0,20);
		glVertex3f(0,0,20);

		glNormal3f(0,-1,0);
	glEnd();

	glRotatef(90, 0,1,0);
	glTranslatef(0,2.5,0);
	gluCylinder(quadric, 2.5, 2.5, 10, 15, 15);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0,0.7f,0,0.5f);
	glTranslatef(-10,5, 5);

	glPushMatrix();

		portalAngle += (360.0f * dt / 1000.0f);
		glRotatef(portalAngle, 0.5f, 0.5f, 0.5f);
		gluSphere(quadric, 2, 5, 5);
		//cout << "portalAngle: " << portalAngle << endl;
	glPopMatrix();

	glRotatef(90,1,0,0);
	glTranslatef(0,0,-5);
	glColor4f(1,1,1,0.25f);
	gluCylinder(quadric, 2.5, 2.5, 9, 15, 15);

	glColor3f(0.8f,0.8f,0.8f);
	glRotatef(-45,0,-1,0);
	glTranslatef(0,0,15);
	//glTranslatef(-10,0,10);
	gluCylinder(quadric, 2.5, 2.5, 9, 15, 15);

	glDisable(GL_BLEND);
	glPopMatrix();
}

void weaponRecoil()
{

}

void drawTest(float dt)
{

	glScalef(10.0f,10.0f,10.0f);
	drawShed();

	glTranslatef(100,0,100);
	for(int asdf = 0; asdf < 2; asdf++)
	{
		glColor3f(0.0f,0.0f,1.0f);
		glPushMatrix();
			glRotatef(itemSpin,0,1,0);
			glutSolidIcosahedron();
		glPopMatrix();
			glTranslatef(100.0f,0,0);

		glPushMatrix();
			glRotatef(itemSpin,0,1,0);
			glutSolidDodecahedron();
		glPopMatrix();
			glTranslatef(100.0f,0,0);

		glPushMatrix();
			glRotatef(itemSpin,0,1,0);
			glutSolidTeapot(10.0f);
		glPopMatrix();
			glTranslatef(100.0f,0,0);

		glPushMatrix();
			glRotatef(itemSpin,0,1,0);
			glutSolidCube(10.0f);
		glPopMatrix();
			glTranslatef(100.0f,0,0);

			if(asdf == 0)
				glTranslatef(-400.0f, 0, 100);
	}
	itemSpin += (360.0f * dt / 1000.0f);
}

/*drawPerson()
{
	
}*/

/*
void setVSync(int interval=1)
{

	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

  const GLubyte* extensions= glGetString(GL_EXTENSIONS);

	if( strstr( reinterpret_cast<const char*>(extensions), "WGL_EXT_swap_control" ) == 0 )
	{
		cout << endl << ">Vsync not supported." << endl;
		cout << endl << endl << "Printing extensions:" <<endl << endl <<extensions << endl << endl;
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n";
	}
	else
	{
	    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

	    if( wglSwapIntervalEXT )
	    {
	    	wglSwapIntervalEXT(interval);
	    	cout << "\n>Vsync set.\n";
	    }
	}	
}
*/
