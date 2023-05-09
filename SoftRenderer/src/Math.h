#ifndef M_MATH_H
#define M_MATH_H

#include <cmath>
#include <algorithm>
#include <iostream>
#include <initializer_list>

struct Vector3f;
struct Vector4f;

struct Vector3f
{
	float x, y, z;
	Vector3f(float x, float y, float z):x(x),y(y),z(z){}
	inline float dotProduct(const Vector3f& v) const;
	inline Vector3f operator-(const Vector3f& v) const;
	inline Vector3f crossProduct(const Vector3f& v) const;
	inline Vector3f mulByVector(const Vector3f& v) const;
	inline Vector3f normalize() const;
	inline explicit operator Vector4f() const;
	inline float length() const;
};

inline Vector3f operator*(float n, const Vector3f& v);

// homogeneous coordinates
struct Vector4f
{
	float x, y, z, w;
	Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline Vector4f operator-(const Vector4f& v) const;
	inline Vector4f regularize() const;
	inline explicit operator Vector3f() const;
};

struct Matrix3f
{
	Matrix3f(std::initializer_list<float> il);
	float num[3 * 3];
};

inline Vector3f operator*(const Matrix3f& m, const Vector3f& v);

struct Matrix4f
{
	Matrix4f(std::initializer_list<float> il);
	float num[4 * 4];
};

inline Vector4f operator*(const Matrix4f& m, const Vector4f& v);

#ifdef _DEBUG
class MathTest
{
public:
	void run();
};
#endif

#endif