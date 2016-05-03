#include "Vector2.h"
#include "sfwdraw.h"
#include <iostream>
#include <cstring>

using std::cout;
using std::endl;

Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(float x1, float y1)
{
	x = x1;
	y = y1;
}

float Vector2::Magnitude()
{
	return sqrtf(x*x + y*y);
}

void Vector2::Normalize()
{
	float tempx = x / Magnitude();
	float tempy = y / Magnitude();

	x = tempx;
	y = tempy;
}

Vector2 Vector2::Normal()
{
	return (*this) / Magnitude();
}

Vector2 Vector2::Perp()
{
	return Vector2(-y, x);
}

float Vector2::GetAngle2D()
{
	float radians = (float)atan2(x, y);
	return (radians / PI) * 180;
}

Vector2 Vector2::SetAngle(float angle)
{
	return Vector2(cosf(angle), sinf(angle));
}

void Vector2::operator=(Vector2 otherVec)
{
	x = otherVec.x;
	y = otherVec.y;
}

Vector2 Vector2::operator+(Vector2 otherVec)
{
	Vector2 temp;
	temp.x = x + otherVec.x;
	temp.y = y + otherVec.y;
	return temp;
}

Vector2 Vector2::operator-(Vector2 otherVec)
{
	Vector2 temp;
	temp.x = x - otherVec.x;
	temp.y = y - otherVec.y;
	return temp;
}

Vector2 Vector2::operator*(Vector2 otherVec)
{
	Vector2 temp;
	temp.x = x * otherVec.x;
	temp.y = y * otherVec.y;
	return temp;
}

Vector2 Vector2::operator/(Vector2 otherVec)
{
	Vector2 temp;
	temp.x = x / otherVec.x;
	temp.y = y / otherVec.y;
	return temp;
}

Vector2 Vector2::operator*(float otherNum)
{
	Vector2 temp;
	temp.x = x * otherNum;
	temp.y = y * otherNum;
	return temp;
}

Vector2 Vector2::operator/(float otherNum)
{
	Vector2 temp;
	temp.x = x / otherNum;
	temp.y = y / otherNum;
	return temp;
}

void Vector2::operator+=(Vector2 otherVec)
{
	x += otherVec.x;
	y += otherVec.y;
}

void Vector2::operator-=(Vector2 otherVec)
{
	x -= otherVec.x;
	y -= otherVec.y;
}

void Vector2::operator*=(Vector2 otherVec)
{
	x *= otherVec.x;
	y *= otherVec.y;
}

void Vector2::operator/=(Vector2 otherVec)
{
	x /= otherVec.x;
	y /= otherVec.y;
}

bool Vector2::operator==(Vector2 otherVec)
{
	//if (x == otherVec.x && y == otherVec.y) return true;
	if (x - FTL_EPSILON < otherVec.x && x + FTL_EPSILON > otherVec.x &&
		y - FTL_EPSILON < otherVec.y && y + FTL_EPSILON > otherVec.y) return true;
	else return false;
}

bool Vector2::operator!=(Vector2 otherVec)
{
	if (x != otherVec.x || y != otherVec.y) return true;
	else return false;
}

Vector2 operator*(float otherNum, Vector2 vec)
{
	Vector2 temp;
	temp.x = vec.x * otherNum;
	temp.y = vec.y * otherNum;
	return temp;
}

Vector2 operator-(Vector2 vec)
{
	Vector2 temp;
	temp.x = -vec.x;
	temp.y = -vec.y;
	return temp;
}

void operator*=(float otherNum, Vector2 vec)
{
	vec.x *= otherNum;
	vec.y *= otherNum;
}