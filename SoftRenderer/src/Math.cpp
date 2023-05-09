#include "Math.h"

Vector3f operator*(float n, const Vector3f& v)
{
	return Vector3f{
		v.x * n,
		v.y * n,
		v.z * n
	};
}

float Vector3f::dotProduct(const Vector3f& v) const
{
	return x * v.x + y * v.y + z * v.z;
}

Vector3f Vector3f::operator-(const Vector3f& v) const
{
	return Vector3f{
		x - v.x,
		y - v.y,
		z - v.z
	};
}

Vector3f Vector3f::crossProduct(const Vector3f& v) const
{
	return Vector3f{
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x
	};
}

Vector3f Vector3f::mulByVector(const Vector3f& v) const
{
	return Vector3f{
		x * v.x,
		y * v.y,
		z * v.z
	};
}

float Vector3f::length() const
{
	return std::sqrtf(x * x + y * y + z * z);
}

Vector3f Vector3f::normalize() const
{
	float len = length();
	return Vector3f{
		x / len,
		y / len,
		z / len
	};
}

Vector3f::operator Vector4f() const
{
	return Vector4f{
		x,y,z,1.0f
	};
}

Vector4f Vector4f::operator-(const Vector4f& v) const
{
	return Vector4f{
		x - v.x,
		y - v.y,
		z - v.z,
		w - v.w
	};
}

Vector4f::operator Vector3f() const
{
	if (fabs(w) < 1e-6)
		return Vector3f{x, y, z};
	return Vector3f{ x/w, y/w, z/w };
}

Vector4f Vector4f::regularize() const
{
	if (fabs(w) < 1e-6)
		return *this;
	
	return Vector4f{
		x / w,
		y / w,
		z / w,
		w / w,
	};
}

Matrix3f::Matrix3f(std::initializer_list<float> il)
{
	std::copy(il.begin(), il.end(), std::begin(num));
}

inline Vector3f operator*(const Matrix3f& m, const Vector3f& v)
{
	return Vector3f{
		m.num[0] * v.x + m.num[1] * v.y + m.num[2] * v.z,
		m.num[3] * v.x + m.num[4] * v.y + m.num[5] * v.z,
		m.num[6] * v.x + m.num[7] * v.y + m.num[8] * v.z
	};
}

Matrix4f::Matrix4f(std::initializer_list<float> il)
{
	std::copy(il.begin(), il.end(), std::begin(num));
}

inline Vector4f operator*(const Matrix4f& m, const Vector4f& v)
{
	return Vector4f{
		m.num[0] * v.x + m.num[1] * v.y + m.num[2] * v.z + m.num[3] * v.w,
		m.num[4] * v.x + m.num[5] * v.y + m.num[6] * v.z + m.num[7] * v.w,
		m.num[8] * v.x + m.num[9] * v.y + m.num[10] * v.z + m.num[11] * v.w,
		m.num[12] * v.x + m.num[13] * v.y + m.num[14] * v.z + m.num[15] * v.w,
	};
}

#ifdef _DEBUG
void MathTest::run()
{
	Vector3f v1 = { 1, 2, 3 };
	Vector3f v2 = { 4, 5, 6 };

	std::cout << " Vector3f dot : " << v1.dotProduct(v2) << std::endl;

	Vector3f t = v1 - v2;
	std::cout << "opeartor - : " << t.x << " " << t.y << " " << t.z << std::endl;

	t = v1.crossProduct(v2);
	std::cout << "cross Product : " << t.x << " " << t.y << " " << t.z << std::endl;

	t = v1.mulByVector(v2);
	std::cout << "mulByVector : " << t.x << " " << t.y << " " << t.z << std::endl;

	t = v1.normalize();
	std::cout << "normalize : " << t.x << " " << t.y << " " << t.z << std::endl;

	std::cout << "length : " << v1.length() << std::endl;

	t = 5 * v1;
	std::cout << "5 * v1 : " << t.x << " " << t.y << " " << t.z << std::endl;

	std::cout << "Vector4f --------------" << std::endl;

	Vector4f v3 = { 2, 4, 6, 2 };
	Vector4f v4 = { 1, 2, 3, 1 };

	Vector4f t2 = v3 - v4;
	std::cout << "v3 - v4 : " << t2.x << " " << t2.y << " " << t2.z << " " << t2.w << std::endl;

	t2 = v3.regularize();
	std::cout << "v3.regularize() : " << t2.x << " " << t2.y << " " << t2.z << " " << t2.w << std::endl;

	t = static_cast<Vector3f>(v3);
	std::cout << "v3 to Vector3f : " << t.x << " " << t.y << " " << t.z << std::endl;

	t2 = static_cast<Vector4f>(v1);
	std::cout << "v1 to Vector4f : " << t2.x << " " << t2.y << " " << t2.z << " " << t2.w << std::endl;

	std::cout << "Matrix --------------" << std::endl;
	Matrix3f m1 = {
		1, 2, 3,
		4, 5, 6,
		7, 8, 9 
	};

	Matrix4f m2 = {
		1, 2, 3, 4,
		5, 6, 7, 8,
		9, 10 , 11, 12,
		13, 14, 15, 16
	};

	t = m1 * v1;
	std::cout << "m1 multiply v1: " << t.x << " " << t.y << " " << t.z << std::endl;

	t2 = m2 * v3;
	std::cout << "m2 multiply v3 : " << t2.x << " " << t2.y << " " << t2.z << " " << t2.w << std::endl;

}

#endif