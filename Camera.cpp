#include "Camera.h"

Camera::Camera(Vec3& p, Vec3& l, Vec3& u) :
	pos(p), look(l), up(u)
{
	right = look.cross(up);
}

void Camera::setView()
{
	
}

Camera::Camera()
{

}

Camera::~Camera()
{
	
}