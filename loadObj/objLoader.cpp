//header
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include "obj.h"
#include "suppliedGlutFuncs.h"

//globals
Obj* myObj = NULL;
GLuint elephant;
float elephantrot;
char ch='1';

bool keyStates[256], keyTaps[256], loadHiRes;

/*--Camera Position and Look Vectors--*/
Vec3 camLook(0.0f,0.0f,-1.0f);
Vec3 camPos(0, 5, -10);
int centerX, centerY;
float add = 0.0f;
bool pause = FALSE;

/*------View-Angles-------*/
float heading = 0.0f;
float pitch = 0.0f;
float perspective = 45.0f;
float aspectRatio = 1024.0f/768.0f; 
float dt = 1.0f;
float camSpeed = 0.5f;

void handleFunc(float dt)
{
  float vecx, vecy, vecz;

  if(keyTaps['.'])
    pause = !pause;

  /*--------change perspective---------*/
  if(keyStates['-'])
  {
    perspective -= 0.01f;
    if(perspective <= 1.0f)
      perspective = 180.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspective, aspectRatio, 0.1f, 10000.0f);
    glMatrixMode(GL_MODELVIEW);
  }
  if(keyStates['\\'])
  {
    perspective= 45.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspective, aspectRatio, 0.1f, 10000.0f);
    glMatrixMode(GL_MODELVIEW);
  }
  if(keyStates['='])
  {
    perspective += 0.01f;
    if(perspective >= 180.0f)
      perspective = 0.0f;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspective, aspectRatio, 0.1f, 10000.0f);
    glMatrixMode(GL_MODELVIEW);
  }

  /*---------Fly-Foward---------*/
  if(keyStates['q'] || keyStates['Q'])
  {
    camPos+=camLook*(1.35f*dt);
  }

  /*--------Move-Foward---------*/
  if(keyStates['w'] || keyStates['W'])
  {

    camPos.x+=camLook.x*(camSpeed*2.0f*dt);
    //camPos.y+=camLook.y;  //comment out to walk forward on plane
    camPos.z+=camLook.z*(camSpeed*2.0f*dt);
  }
  if(keyStates['r'] || keyStates['R'])
  {
    camPos.x+=camLook.x*(camSpeed*2.0f*dt);
    //camPos.y+=camLook.y;  //comment out to walk forward on plane
    camPos.z+=camLook.z*(camSpeed*2.0f*dt);
  }
  /*--------Move-Backwards------*/
  if(keyStates['s'] || keyStates['S'])
  {
    camPos.x-=camLook.x*(camSpeed*2.0f*dt);
    //y-=ly*speed;  //comment out to walk backwards on plane
    camPos.z-=camLook.z*(camSpeed*2.0f*dt);
  }
  /*--------Move-Left----------*/
  if(keyStates['a'] || keyStates['A'])
  {
    vecx = camLook.z;
    vecy = camLook.y;
    vecz = -camLook.x;
    camPos.x+= vecx*(camSpeed*2.0f*dt);
    camPos.z+= vecz*(camSpeed*2.0f*dt);
  }
  /*--------Move-Right----------*/
  if(keyStates['d'] || keyStates['D'])
  {

    vecx = camLook.z;
    vecy = camLook.y;
    vecz = -camLook.x;
    camPos.x-= vecx*(camSpeed*2.0f*dt);
    camPos.z-= vecz*(camSpeed*2.0f*dt);
  }

  if(keyStates['p'] || keyStates['P'])
  {
    add+= 0.2f;
  }
  if(keyStates['o'] || keyStates['O'])
  {
    add-= 0.2f;
  }

  if(keyStates['x'] || keyStates['X'])
    camPos.y += (camSpeed*1.30f*dt);

  if(keyStates['Z'] || keyStates['z'])
    camPos.y -= (camSpeed*1.30f*dt);
}

//other functions and main
void cleanUp();

//.obj loader code begins
void loadObj(const char *fname)
{

  FILE *fp;
  int read;
  GLfloat x, y, z;
  char ch;
  elephant=glGenLists(1);
  fp=fopen(fname,"r");
  if (!fp) 
      {
          printf("can't open file %s\n", fname);
  	      exit(1);
      }
  glPointSize(2.0);
  glNewList(elephant, GL_COMPILE);

  glPushMatrix();
  glBegin(GL_POINTS);
  while(!(feof(fp)))
  {
    read=fscanf(fp,"%c %f %f %f",&ch,&x,&y,&z);
    if(read==4&&ch=='v')
    {
      glVertex3f(x,y,z);
    }
  }
  glEnd();

  glPopMatrix();
  glEndList();
  fclose(fp);
}
//.obj loader code ends here

void draw()
{
 	glPushMatrix();
//  glTranslatef(0,0,30);
 	glTranslatef(0,-40.00,-105);
// 	glColor3f(1.0,0.23,0.27);
 	
  glScalef(85.0f+add,85.0f+add,85.0f+add);
  //glScalef(0.001f,0.001f,0.001f);

  if(pause)
    elephantrot = 0;

 	glRotatef(elephantrot,0,1,0);
  myObj->draw();

// 	glCallList(elephant);
 	glPopMatrix();
 	elephantrot=elephantrot+0.1f;
 	if(elephantrot>360)elephantrot=elephantrot-360;
}

void display(void)
{  

   	glClearColor (0.0,0.0,0.0,1.0); 
   	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    handleFunc(dt);
    glLoadIdentity();
    gluLookAt(camPos.x,camPos.y,camPos.z,camPos.x+camLook.x,camPos.y+camLook.y,camPos.z+camLook.z,0,1,0);
   	draw();
    glutSwapBuffers(); //swap the buffers

      memset(keyTaps,0,256*sizeof(bool));

}

int main(int argc, char** argv)
{
  if(argc == 1)
  {
    loadHiRes = FALSE;
    cout <<"> Loading Lo-Res textures ..." << endl;
  }
  else
  {
    if(argv[1][0] == '0')
    {
      cout <<"> Loading Lo-Res textures ..." << endl;
      loadHiRes = FALSE;
    }
    else if(argv[1][0] == '1')
    {
      cout <<"> Loading Hi-Res textures ..." << endl;
      loadHiRes = TRUE;
    }
    else 
    {
      cout <<"> Invalid arguements, loading Lo-Res textures ..." << endl;
      loadHiRes = FALSE;
    }
  }
  initGL(argc, argv);
	
  atexit(cleanUp);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

  myObj = new Obj("./data/stormtrooper/Stormtrooper.obj"); 


  //loadObj("data/newgun.obj");//replace porsche.obj with radar.obj or any other .obj to display it
	glutMainLoop();
	return 0;
}

void cleanUp()
{
  if(myObj)
  delete myObj;
  
  myObj = NULL;
}
