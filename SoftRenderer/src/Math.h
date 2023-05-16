#ifndef M_MATH_H
#define M_MATH_H

#include <cmath>
#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <vector>
#include <algorithm>
#include <cassert>

const float PI = 3.1415926;

struct Vector3f;
struct Vector4f;

struct Vector3i
{
	int x, y, z;
};

struct Vector2f
{
	float x, y;
	Vector2f operator-(const Vector2f& v) const;
	Vector2f operator-()const;
	Vector2f operator+(const Vector2f& v) const;
};

Vector2f operator*(float n, const Vector2f& v);
Vector2f operator/(const Vector2f& v, float n);


struct Vector3f
{
	float x, y, z;
	Vector3f():x(0.f),y(0.f),z(0.f){}
	Vector3f(std::initializer_list<float> il);
	explicit Vector3f(float x, float y, float z):x(x),y(y),z(z){}
	float dotProduct(const Vector3f& v) const;
	Vector3f operator-(const Vector3f& v) const;
	Vector3f operator-()const;
	Vector3f operator+(const Vector3f& v) const;
	Vector3f crossProduct(const Vector3f& v) const;
	Vector3f mulByVector(const Vector3f& v) const;
	Vector3f normalize() const;
	explicit operator Vector4f() const;
	float length() const;
	float squreLen() const;
};

Vector3f operator*(float n, const Vector3f& v);
Vector3f operator/(const Vector3f& v, float n);

// homogeneous coordinates
struct Vector4f
{
	float x, y, z, w;
	Vector4f() :x(0.f), y(0.f), z(0.f) {}
	Vector4f(std::initializer_list<float> il);
	explicit Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4f operator-(const Vector4f& v) const;
	Vector4f operator-()const;
	Vector4f operator+(const Vector4f& v) const;
	Vector4f regularize() const;
	explicit operator Vector3f() const;
};

Vector4f operator*(float n, const Vector4f& v);
Vector4f operator/(const Vector4f& v, float n);

struct Matrix3f
{
	Matrix3f() :num{0.f} {};
	Matrix3f(std::initializer_list<float> il);
	float determinant() const;
	Matrix3f transpose() const;
	Matrix3f inverse() const;
	Matrix3f adjugate() const;
	
	static Matrix3f Identity();
	float num[3 * 3];
};

Vector3f operator*(const Matrix3f& m, const Vector3f& v);
Matrix3f operator*(const Matrix3f& m1, const Matrix3f& m2);
Matrix3f operator*(float n, const Matrix3f& m);

struct Matrix4f
{
	Matrix4f() :num{ 0.f } {};
	Matrix4f(std::initializer_list<float> il);
	float determinant() const;
	Matrix4f transpose() const;
	Matrix4f inverse() const;
	Matrix4f adjugate() const;

	static Matrix4f Identity();
	static Matrix4f Zero();
	float num[4 * 4];
private:
	Matrix3f getMij(int i, int j) const;
};

Vector4f operator*(const Matrix4f& m, const Vector4f& v);
Matrix4f operator*(const Matrix4f& m1, const Matrix4f& m2);
Matrix4f operator+(const Matrix4f& m1, const Matrix4f& m2);
Matrix4f operator*(float n, const Matrix4f& m);

class MathUtility
{
public:
	template<typename T>
	static T clamp(const T& val, const T& minVal, const T& maxVal)
	{
		return val < minVal ? minVal : (val > maxVal ? maxVal : val);
	}

	template<typename T>
	static T interpolateByBaryCentric(T v[3], const Vector4f* triPos, float alpha, float beta, float gamma)
	{
		auto wa = alpha / triPos[0].w;
		auto wb = beta / triPos[1].w;
		auto wc = gamma / triPos[2].w;

		return (wa * v[0] + wb * v[1] + wc * v[2]) / (wa + wb + wc);
	}

	static Matrix4f getPerspctiveMatrix(float fov, float aspectRatio, float zNear, float zFar);

	// 
	static Matrix4f rotateX(const float angle);
	static Matrix4f rotateY(const float angle);
	static Matrix4f rotateZ(const float angle);
};

#ifdef _DEBUG
class MathTest
{
public:
	void run();
};
#endif

#endif