#include "Portal.h"
extern int Portals, PortalsCenter, PortalsOuter;
extern float dt;


Portal::Portal(Vec3 CamPos, Vec3 CamLook, chrono::steady_clock::time_point START) :
 start(START)
{
	pos = CamPos;
	look = CamLook;

	chrono::milliseconds tmp = chrono::milliseconds(20000);
	end = start + tmp;
	close = start + chrono::milliseconds(15000);
	open = start + chrono::milliseconds(1000);
	increment = 0;
	increment2 = 1;

	//cout << "> Portal start: "<< start<<"\t Portal end: "<< end << endl;
}

void Portal::texturePortal(int testTexture)
{
	/*-------test texturing----*/
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, testTexture);
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-50.0f, 0.0f, 50.0f);
		glTexCoord2f(1, 0);
		glVertex3f( 50.0f, 0.0f, 50.0f);
		glTexCoord2f(1, 1);
		glVertex3f( 50.0f, 0.0f, -50.0f);
		glTexCoord2f(0, 1);
		glVertex3f(-50.0f, 0.0f, -50.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Portal::openPortal()
{
	chrono::milliseconds tmp;

		tmp = chrono::milliseconds(1500);

	auto now = chrono::steady_clock::now();
	auto stopPortal = start + tmp;

	if(now < stopPortal)
	{
		pos.x+=look.x*5;
		pos.y+=look.y*5;
		pos.z+=look.z*5;
	}
/*	 if(now < stopPortal)
	{
		pos.x+=look.x;
		pos.y+=look.y;
		pos.z+=look.z;
	}*/

	normalizePortal();
	glPushMatrix();
		
		glMultMatrixf(orientationMatrix);
		if(now > close)
		{
			glScalef(1-increment,1-increment,1-increment);
			if(increment < 1)
				increment+= 0.001*dt;
		}
		if(now < open && now > stopPortal)
		{
			glScalef(1-increment2,1-increment2,1-increment2);
			if(increment2 > 0)
				increment2-= 0.001f*dt;
		}
		if(now < stopPortal)
		{
			glScalef(1-increment2,1-increment2,1-increment2);
			if(increment2 > 0)
			{
				increment2-= 0.01f*dt;
			}
		}

		glRotatef(90,1,0,0);
		glScalef(2.5f,2.5f,2.5f);
		texturePortal(Portals);

		portalAngle += (180.0f * dt / 1000.0f);
		glRotatef(portalAngle, 0, 1, 0);
		glScalef(0.375f,0.375f,0.375f);
		glTranslatef(1.0f,1.0f,1.0f);
		texturePortal(PortalsCenter);
		glTranslatef(-2.0f,-2.0f,-2.0f);
		texturePortal(PortalsCenter);
	glPopMatrix();
}

void Portal::normalizePortal()
{
	Vec3 tmpUp(0,1,0);
	right = look.cross(tmpUp);
	up = right.cross(look);

	memset(orientationMatrix, 0, sizeof(float)*16);

	orientationMatrix[15]=1.0f;
	orientationMatrix[0]=right.x;
	orientationMatrix[1]=right.y;
	orientationMatrix[2]=right.z;
	orientationMatrix[4]=up.x;
	orientationMatrix[5]=up.y;
	orientationMatrix[6]=up.z;
	orientationMatrix[8]=-look.x;
	orientationMatrix[9]=-look.y;
	orientationMatrix[10]=-look.z;
	orientationMatrix[12]=pos.x;
	orientationMatrix[13]=pos.y;
	orientationMatrix[14]=pos.z;

}

