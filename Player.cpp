#include "Player.h"

Player::Player(const char* fileName, const char* Name, Vec3& initialPos, float eh, Vec3& lv) :
	Obj(fileName, Name), deaths(0), eyeHeight(eh), bodyCenter(0), health(100.0f), speed(1.0f), 
	alive(true), deadCam(false)
{ 
	name = Name;
	pos = initialPos;
	look = lv;
	type = "player";
}

Player::Player()
{
	type = "player";
}

float Player::eye()
{
	return eyeHeight;
}
void Player::updateCamera()
{
}
void Player::persue(float dt)
{
	lookAt(target->pos);
	float dist = pos.distance(target->pos);

	/*----Persue if distance > 20
		  if less than 20, attack-------*/
	if(dist > 20.0f)
		pos += (look*dt*0.1f*speed);
	else
	{
		if(dist <= 20.0f)
			attack(target);
	}
}
void Player::attack(Player* _target)
{
	if(!readyToAttack)
	{
		auto nextAttackTime = lastAttackTime + chrono::milliseconds(2000);
		auto tmpnow = chrono::steady_clock::now();

		if(nextAttackTime < tmpnow)
			readyToAttack = true;
	}

		if(readyToAttack == true && _target->isAlive())
		{
			_target->health -= 20.0f;
			readyToAttack = false;
			lastAttackTime = chrono::steady_clock::now();
			if(attackGrunt != NULL)
				Mix_PlayChannel( -1, attackGrunt, 0 );

			if(_target->health <= 0.0f)
				_target->setDead();
		}
}
void Player::setTarget(Player* _target)
{
	target = _target;
}

void Player::setAlive()
{
	alive = true;
	health = 100.0f;
	setRot(0,0,0,0);
}

void Player::setBodyCenter(float bc)
{
	bodyCenter = bc;
}

void Player::setGrunt(const char* fileLoc)
{
	attackGrunt = Mix_LoadWAV(fileLoc);
}

Vec3 Player::returnCollisionVec()
{
	return Vec3(pos.x, pos.y + bodyCenter, pos.z);
}

Player::~Player()
{
	
}