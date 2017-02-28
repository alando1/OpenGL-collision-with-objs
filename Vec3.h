#pragma once

#include <cmath>

using namespace std;


#ifndef PI
#define PI	3.14159265358979325f
#endif

#ifndef PI_OVER_180
#define PI_OVER_180  (PI / 180.0f)
#endif

#ifndef EPSILON
#define EPSILON	0.000001f
#endif

//------------------------------------------------------------------------------

class Vec2
{
public:
	float x, y;

	Vec2() { }
	Vec2(float newX, float newY) { x = newX; y = newY; }
	Vec2(float* v) { x = v[0]; y = v[1]; }
	Vec2(const Vec2& v) { x = v.x; y = v.y; }

	// Array access
	float& operator[](int idx) const
	{
		return ((float*)this)[idx];
	}
	
	// Assignment
	Vec2& operator=(const Vec2& v)
	{
		x = v.x;
		y = v.y;
	
		return *this;
	}
	
	// Equality
	bool operator==(const Vec2& v) const
	{
		return ((x == v.x) && (y == v.y));
	}
	
	// Inequality
	bool operator!=(const Vec2& v) const
	{
		return ((x != v.x) || (y != v.y));
	}
	
	// Addition
	Vec2 operator+(const Vec2& v) const
	{
		return Vec2(x + v.x, y + v.y);
	}
	
	// Increment
	Vec2& operator+=(const Vec2& v)
	{
		x += v.x;
		y += v.y;
	
		return *this;
	}
	
	// Subtraction
	Vec2 operator-(const Vec2& v) const
	{
		return Vec2(x - v.x, y - v.y);
	}
	
	// Negation
	Vec2 operator-() const
	{
		return Vec2(-x, -y);
	}
	
	// Decrement
	Vec2& operator-=(const Vec2& v)
	{
		x -= v.x;
		y -= v.y;
	
		return *this;
	}
	
	// Scalar multiply and assignment
	Vec2& operator*=(float s)
	{
		x *= s;
		y *= s;
	
		return *this;
	}
	
	// Scalar division and assignment
	Vec2& operator/=(float s)
	{
		float r = 1.0f / s;
	
		x *= r;
		y *= r;
	
		return *this;
	}
	
	// Scalar multiply
	Vec2 operator*(float s) const
	{
		return Vec2(x * s, y * s);
	}
	
	// Scalar divide
	Vec2 operator/(float s) const
	{
		float r = 1.0f / s;
	
		return Vec2(x * r, y * r);
	}
	
	// Dot product
	float dot(const Vec2& v) const
	{
		return x * v.x + y * v.y;
	}
	
	// Length
	float length() const
	{
		return sqrtf(x * x + y * y);
	}
	
	// Length squared
	float lengthSqr() const
	{
		return (x * x + y * y);
	}
	
	// Normalize
	Vec2& normalize()
	{
		float r = 1.0f / length();
		x *= r;
		y *= r;

		return *this;
	}
};

//------------------------------------------------------------------------------

class Vec3
{
	public:
		float x, y, z;
	
		Vec3() { }
		Vec3(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ; }
		Vec3(float* v) { x = v[0]; y = v[1]; z = v[2]; }
		Vec3(const Vec3& v) { x = v.x; y = v.y; z = v.z; }

		// Array access
		float& operator[](int idx)
		{
			return ((float*)this)[idx];
		}
	
		// Assignment
		Vec3& operator=(const Vec3& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
	
			return *this;
		}
	
		// Equality
		bool operator==(const Vec3& v) const
		{
			return ((x == v.x) && (y == v.y) && (z == v.z));
		}
	
		// Inequality
		bool operator!=(const Vec3& v) const
		{
			return ((x != v.x) || (y != v.y) || (z != v.z));
		}

		bool isEqualEpsilon(const Vec3& v) const
		{
			return (fabs(x - v.x) < EPSILON) && (fabs(y - v.y) < EPSILON) && (fabs(z - v.z) < EPSILON);
		}

		bool isNotEqualEpsilon(const Vec3& v) const
		{
			return !isEqualEpsilon(v);
		}
	
		// Addition
		Vec3 operator+(const Vec3& v) const
		{
			return Vec3(x + v.x, y + v.y, z + v.z);
		}
	
		// Increment
		Vec3& operator+=(const Vec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
	
			return *this;
		}
	
		// Subtraction
		Vec3 operator-(const Vec3& v) const
		{
			return Vec3(x - v.x, y - v.y, z - v.z);
		}
	
		// Negation
		Vec3 operator-() const
		{
			return Vec3(-x, -y, -z);
		}
	
		// Decrement
		Vec3& operator-=(const Vec3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
	
			return *this;
		}
	
		// Scalar multiply and assignment
		Vec3& operator*=(float s)
		{
			x *= s;
			y *= s;
			z *= s;
	
			return *this;
		}
	
		// Scalar division and assignment
		Vec3& operator/=(float s)
		{
			float r = 1.0f / s;
	
			x *= r;
			y *= r;
			z *= r;
	
			return *this;
		}
	
		// Scalar multiply
		Vec3 operator*(float s) const
		{
			return Vec3(x * s, y * s, z * s);
		}
	
		// Scalar divide
		Vec3 operator/(float s) const
		{
			float r = 1.0f / s;
	
			return Vec3(x * r, y * r, z * r);
		}
	
		// Dot product
		float dot(const Vec3& v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}
	
		// Dot product vs. float[3]
		float dot(const float* v) const
		{
			return x * v[0] + y * v[1] + z * v[2];
		}
	
		// Cross product
		Vec3 cross(const Vec3& v) const
		{
			return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}
	
		// Length
		float length() const
		{
			return sqrtf(x * x + y * y + z * z);
		}
	
		// Length squared
		float lengthSqr() const
		{
			return (x * x + y * y + z * z);
		}
	
		// Normalize
		Vec3& normalize()
		{
			float r = 1.0f / length();
			x *= r;
			y *= r;
			z *= r;

			return *this;
		}

		//Distance
		float distance(Vec3 initial) const
		{
			return sqrtf(
						(this->x - initial.x) * (this->x - initial.x) + 
						(this->y - initial.y) * (this->y - initial.y) +
						(this->z - initial.z) * (this->z - initial.z)
						);
		}
};
