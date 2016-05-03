#pragma once
#define FTL_EPSILON 0.0001f
#define RAD2DEG 57.2958f
#define DEG2RAD 0.0174533f

struct Vector2
{
public:
	float x;
	float y;
	Vector2();
	Vector2(float x1, float y1);
	float Magnitude();
	void Normalize();
	Vector2 Normal();
	Vector2 Perp();
	float GetAngle2D();
	static Vector2 SetAngle(float angle);

	void operator=(Vector2 otherVec);
	Vector2 operator+(Vector2 otherVec);
	Vector2 operator-(Vector2 otherVec);
	Vector2 operator*(Vector2 otherVec);
	Vector2 operator/(Vector2 otherVec);
	Vector2 operator*(float otherNum);
	Vector2 operator/(float otherNum);
	void operator+=(Vector2 otherVec);
	void operator-=(Vector2 otherVec);
	void operator*=(Vector2 otherVec);
	void operator/=(Vector2 otherVec);
	bool operator==(Vector2 otherVec);
	bool operator!=(Vector2 otherVec);
};
Vector2 operator*(float otherNum, Vector2 vec);
Vector2 operator-(Vector2 vec);
void operator*=(float otherNum, Vector2 vec);

