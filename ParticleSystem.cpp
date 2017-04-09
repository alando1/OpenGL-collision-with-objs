#include "ParticleSystem.h"


void ParticleSystem::update(float dt)
{
	switch (_type)
	{
	case PST_BLOOM:
		_updateBloom(dt);
		break;
	default:
		break;
	};
}

void ParticleSystem::reset()
{

}

void ParticleSystem::render(const glm::vec3& camPos)
{
	/*
	glPointSize(5.0f);
	glBegin(GL_POINTS);
		for (int i = 0; i < _particleCount; ++i)
		{
			glVertex3f(_particles[i].pos.x, _particles[i].pos.y, _particles[i].pos.z);
		}
	glEnd();
	glPointSize(1.0f);
	*/

	glm::vec3 look, right, tmpUp(0.0f, 1.0f, 0.0f), up;
	float m[16];

	//glTranslatef(_origin.x, _origin.y, _origin.z);

	
	
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, _particleTexture);
	
	
	for (int i = 0; i < _particleCount; ++i)
	{
		Particle* p = &_particles[i];

		look = p->pos - camPos;
		right = glm::cross(look, tmpUp);
		up = glm::cross(right, look);

		look = glm::normalize(look);
		right = glm::normalize(right);
		up = glm::normalize(up);
		/*
		m[0] = right.x;
		m[1] = right.y;
		m[2] = right.z;
		m[3] = 0.0f;

		m[4] = up.x;
		m[5] = up.y;
		m[6] = up.z;
		m[7] = 0.0f;

		m[8] = -look.x;
		m[9] = -look.y;
		m[10] = -look.z;
		m[11] = 0.0f;

		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;
		*/
		glPushMatrix();
			//glTranslatef(p->pos.x, p->pos.y, p->pos.z);
			//glMultMatrixf(m);

			//glEnable(GL_ALPHA_TEST);
			//glAlphaFunc(GL_NOTEQUAL, 0);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, _particleTexture);

			float scale = 0.6f;
			glColor4f(0.1f, 0.9f, 0.8f, p->alpha);
			glBegin(GL_QUADS);
				glm::vec3 p0, p1, p2, p3;
				p0 = p->pos + right * scale - up * scale;
				p1 = p->pos + right * scale + up * scale;
				p2 = p->pos - right * scale + up * scale;
				p3 = p->pos - right * scale - up * scale;

				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(p0.x, p0.y, p0.z);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(p1.x, p1.y, p1.z);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(p2.x, p2.y, p2.z);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(p3.x, p3.y, p3.z);
			glEnd();
		glPopMatrix();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		//glDisable(GL_ALPHA_TEST);
		glDepthMask(GL_TRUE);
		
		/*
		glBegin(GL_LINES);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f(p->pos.x, p->pos.y, p->pos.z);
			glVertex3f(p->pos.x + right.x, p->pos.y + right.y, p->pos.z + right.z);

			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f(p->pos.x, p->pos.y, p->pos.z);
			glVertex3f(p->pos.x + up.x, p->pos.y + up.y, p->pos.z + up.z);

			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3f(p->pos.x, p->pos.y, p->pos.z);
			glVertex3f(p->pos.x - look.x, p->pos.y - look.y, p->pos.z - look.z);
		glEnd();
		*/
	}


	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ParticleSystem::destroy()
{
	if (_particles != NULL)
	{
		delete[] _particles;
		_particles = NULL;
	}

	if (_particleTexture != 0)
	{
		glDeleteTextures(1, &_particleTexture);
		_particleTexture = 0;
	}
}

void ParticleSystem::_build(string& texturePath)
{
	_particles = new Particle[_particleCount];
	_particleTexture = SOIL_load_OGL_texture(texturePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

	// vertex data will be rendered as GL_QUADS, so 4 verts per quad, 3 floats per vert, 12 total per particle
	//_verts = new float[_particleCount * 12];
	//_texCoords = new float[_particleCount * 8];


}

void ParticleSystem::_initParticleBloom(Particle* p)
{
	p->pos = _origin;

	// generate a random point inside a disc of radius 2, centered at 0,5,0.
	// normalize this and it becomes the particle's velocity
	float theta = static_cast<float>(rand()) / RAND_MAX * 360.0f;
	float radius = static_cast<float>(rand()) / RAND_MAX * 2.0f;
	glm::vec3 r;
	r.x = cosf(theta * PI_OVER_180) * radius;
	r.y = 0.0f;
	r.z = sinf(theta * PI_OVER_180) * radius;

	r += glm::vec3(0.0f, 5.0f, 0.0f);
	r = glm::normalize(r);
	p->vel = r * (static_cast<float>(rand()) / RAND_MAX * 20.0f + 1.0f);

	// other defaults
	p->alpha = 1.0f;
	p->lifeTime = 0.0f;
}

/*
	Updater for a 'bloom' style of particle system, where points originate from a central spot and fly upwards
	in a ballistic path under the influence of gravity.	
*/
void ParticleSystem::_updateBloom(float dt)
{
	// initial state to set before renderloop
	if (dt == 0.0f)
	{
		for (int i = 0; i < _particleCount; ++i)
			_initParticleBloom(&_particles[i]);
	}
	// general update case
	else
	{
		glm::vec3 gravity(0.0f, -9.81f, 0.0f);

		for (int i = 0; i < _particleCount; ++i)
		{
			Particle* p = &_particles[i];
			p->vel += (gravity * dt / 1000.0f);
			p->pos += (p->vel * dt / 1000.0f);

			/*
			// this is dumb
			p->lifeTime += dt;
			p->alpha = (3.0f - p->lifeTime / 1000.0f) / 3.0f;
			if (p->alpha < 0.0f)
				p->alpha = 0.0f;
			*/

			// reset the particle if it drops below the xz plane
			if (p->pos.y < 0.0f)
				_initParticleBloom(p);
		}
	}
}