#include "Math.h"

Vector2f Vector2f::operator-(const Vector2f& v) const
{
	return Vector2f{
		x - v.x,
		y - v.y,
	};
}
Vector2f Vector2f::operator-()const
{
	return Vector2f{
		-x, -y,
	};
}

Vector2f Vector2f::operator+(const Vector2f& v) const
{
	return Vector2f{
		x + v.x,
		y + v.y,
	};
}

Vector2f operator*(float n, const Vector2f& v)
{
	return Vector2f{
		v.x * n,
		v.y * n,
	};
}
Vector2f operator/(const Vector2f& v, float n)
{
	return Vector2f{
		v.x / n,
		v.y / n,
	};
}

Vector3f operator*(float n, const Vector3f& v)
{
	return Vector3f{
		v.x * n,
		v.y * n,
		v.z * n
	};
}

Vector3f operator/(const Vector3f& v, float n)
{
	return Vector3f{
		v.x / n,
		v.y / n,
		v.z / n
	};
}

Vector3f Vector3f::operator-()const
{
	return Vector3f{
		-x,
		-y,
		-z,
	};
}

float Vector3f::squreLen() const
{
	return x * x + y * y + z * z;
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
		x,y,z,0.0f
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

Vector4f Vector4f::operator-() const
{
	return Vector4f{
		-x,
		-y,
		-z
	};
}

Vector4f::operator Vector3f() const
{
	return Vector3f{ x, y, z };
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

Vector3f::Vector3f(std::initializer_list<float> il)
{
	assert(il.size() >= 3);
	auto it = il.begin();
	x = *it;
	++it;
	y = *it;
	++it;
	z = *it;
}

Vector4f::Vector4f(std::initializer_list<float> il)
{
	assert(il.size() >= 4);
	auto it = il.begin();
	x = *it;
	++it;
	y = *it;
	++it;
	z = *it;
	++it;
	w = *it;
}

Matrix3f::Matrix3f(std::initializer_list<float> il)
{
	std::copy(il.begin(), il.end(), std::begin(num));
}

Vector3f operator*(const Matrix3f& m, const Vector3f& v)
{
	return Vector3f{
		m.num[0] * v.x + m.num[1] * v.y + m.num[2] * v.z,
		m.num[3] * v.x + m.num[4] * v.y + m.num[5] * v.z,
		m.num[6] * v.x + m.num[7] * v.y + m.num[8] * v.z
	};
}

Matrix3f operator*(const Matrix3f& m1, const Matrix3f& m2)
{
	return Matrix3f{
		m1.num[0] * m2.num[0] + m1.num[1] * m2.num[3] + m1.num[2] * m2.num[6],
		m1.num[0] * m2.num[1] + m1.num[1] * m2.num[4] + m1.num[2] * m2.num[7],
		m1.num[0] * m2.num[2] + m1.num[1] * m2.num[5] + m1.num[2] * m2.num[8],

		m1.num[3] * m2.num[0] + m1.num[4] * m2.num[3] + m1.num[5] * m2.num[6],
		m1.num[3] * m2.num[1] + m1.num[4] * m2.num[4] + m1.num[5] * m2.num[7],
		m1.num[3] * m2.num[2] + m1.num[4] * m2.num[5] + m1.num[5] * m2.num[8],

		m1.num[6] * m2.num[0] + m1.num[7] * m2.num[3] + m1.num[8] * m2.num[6],
		m1.num[6] * m2.num[1] + m1.num[7] * m2.num[4] + m1.num[8] * m2.num[7],
		m1.num[6] * m2.num[2] + m1.num[7] * m2.num[5] + m1.num[8] * m2.num[8],
	};
}

Matrix3f Matrix3f::Identity()
{
	return Matrix3f{
		1.0f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f
	};
}

Matrix4f Matrix4f::Identity()
{
	return Matrix4f{
		1.0f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f,
	};
}

float Matrix3f::determinant() const
{
	return num[0] * num[4] * num[8] + num[1] * num[5] * num[6] + num[2] * num[3] * num[7]
		- num[2] * num[4] * num[6] - num[1] * num[3] * num[8] - num[0] * num[5] * num[7];
}

Matrix3f operator*(float n, const Matrix3f& m)
{
	return Matrix3f{
		m.num[0] * n, m.num[1] * n, m.num[2] * n,
		m.num[3] * n, m.num[4] * n, m.num[5] * n,
		m.num[6] * n, m.num[7] * n, m.num[8] * n,
	};
}

Matrix3f Matrix3f::transpose() const
{
	return Matrix3f{
		num[0], num[3], num[6],
		num[1], num[4], num[7],
		num[2], num[5], num[8],
	};
}

Matrix3f Matrix3f::adjugate() const
{
	return Matrix3f{
		num[4] * num[8] - num[5] * num[7],
		-(num[3] * num[8] - num[5] * num[6]),
		num[3] * num[7] - num[4] * num[6],

		-(num[1] * num[8] - num[2] * num[7]),
		num[0] * num[8] - num[2] * num[6],
		-(num[0] * num[7] - num[1] * num[6]),

		num[1] * num[5] - num[2] * num[4],
		-(num[0] * num[5] - num[2] * num[3]),
		num[0] * num[4] - num[1] * num[3],
	}.transpose();
}

Matrix3f Matrix3f::inverse() const
{
	auto det = determinant();
	assert(fabs(det) >= 1e-6);

	Matrix3f adj = adjugate();
	return  (1.0f / det) * adj ;
}

Matrix4f Matrix4f::adjugate() const
{
	std::vector<float> nums;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			auto Aij = getMij(i, j).determinant() * ((i + j) % 2 ? -1 : 1);
			nums.push_back(Aij);
		}
	}

	Matrix4f res = { 0 };
	std::copy(nums.begin(), nums.end(), std::begin(res.num));

	return res.transpose();
}

float Matrix4f::determinant() const
{
	Matrix3f M11 = {
		num[5], num[6], num[7],
		num[9], num[10], num[11],
		num[13], num[14], num[15],
	};

	Matrix3f M12 = {
		num[4], num[6], num[7],
		num[8], num[10], num[11],
		num[12], num[14], num[15],
	};

	Matrix3f M13 = {
		num[4], num[5], num[7],
		num[8], num[9], num[11],
		num[12], num[13], num[15],
	};

	Matrix3f M14 = {
		num[4], num[5], num[6],
		num[8], num[9], num[10],
		num[12], num[13], num[14],
	};

	return num[0] * M11.determinant() - num[1] * M12.determinant() 
		 + num[2] * M13.determinant() - num[3] * M14.determinant();
}
Matrix4f Matrix4f::transpose() const
{
	return Matrix4f{
		num[0], num[4], num[8], num[12],
		num[1], num[5], num[9], num[13],
		num[2], num[6], num[10], num[14],
		num[3], num[7], num[11], num[15],
	};
}

Vector4f operator*(float n, const Vector4f& v)
{
	return Vector4f{
		v.x * n,
		v.y * n,
		v.z * n,
		v.w * n,
	};
}

Vector3f Vector3f::operator+(const Vector3f& v) const
{
	return Vector3f{
		v.x + x,
		v.y + y,
		v.z + z,
	};
}

Matrix4f operator+(const Matrix4f& m1, const Matrix4f& m2)
{
	return Matrix4f{
		m1.num[0] + m2.num[0], m1.num[1] + m2.num[1], m1.num[2] + m2.num[2], m1.num[3] + m2.num[3],
		m1.num[4] + m2.num[4], m1.num[5] + m2.num[5], m1.num[6] + m2.num[6], m1.num[7] + m2.num[7],
		m1.num[8] + m2.num[8], m1.num[9] + m2.num[9], m1.num[10] + m2.num[10], m1.num[11] + m2.num[11],
		m1.num[12] + m2.num[12], m1.num[13] + m2.num[13], m1.num[14] + m2.num[14], m1.num[15] + m2.num[15],
	};
}
Matrix4f Matrix4f::Zero()
{
	return Matrix4f{
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
	};
}

Vector4f Vector4f::operator+(const Vector4f& v) const
{
	return Vector4f{
		v.x + x,
		v.y + y,
		v.z + z,
		v.w + w,
	};
}

Vector4f operator/(const Vector4f& v, float n)
{
	return Vector4f{
		v.x / n,
		v.y / n,
		v.z / n,
		v.w / n
	};
}

Matrix3f Matrix4f::getMij(int i, int j) const
{
	std::vector<float> nums;
	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			if (x != i && y != j)
				nums.push_back(num[x * 4 + y]);
		}
	}

	Matrix3f res = {0};
	std::copy(nums.begin(), nums.end(), std::begin(res.num));

	return res;
}

Matrix4f operator*(float n, const Matrix4f& m)
{
	return Matrix4f{
		m.num[0] * n, m.num[1] * n, m.num[2] * n, m.num[3] * n,
		m.num[4] * n, m.num[5] * n, m.num[6] * n, m.num[7] * n,
		m.num[8] * n, m.num[9] * n, m.num[10] * n, m.num[11] * n,
		m.num[12] * n, m.num[13] * n, m.num[14] * n, m.num[15] * n,
	};
}

Matrix4f Matrix4f::inverse() const
{
	auto det = determinant();
	assert(fabs(det) >= 1e-6);

	Matrix4f adj = adjugate();
	return (1.0f / det) * adj;
}

Matrix4f operator*(const Matrix4f& m1, const Matrix4f& m2)
{
	return Matrix4f{
		m1.num[0] * m2.num[0] + m1.num[1] * m2.num[4] + m1.num[2] * m2.num[8] + m1.num[3] * m2.num[12],
		m1.num[0] * m2.num[1] + m1.num[1] * m2.num[5] + m1.num[2] * m2.num[9] + m1.num[3] * m2.num[13],
		m1.num[0] * m2.num[2] + m1.num[1] * m2.num[6] + m1.num[2] * m2.num[10] + m1.num[3] * m2.num[14],
		m1.num[0] * m2.num[3] + m1.num[1] * m2.num[7] + m1.num[2] * m2.num[11] + m1.num[3] * m2.num[15],

		m1.num[4] * m2.num[0] + m1.num[5] * m2.num[4] + m1.num[6] * m2.num[8] + m1.num[7] * m2.num[12],
		m1.num[4] * m2.num[1] + m1.num[5] * m2.num[5] + m1.num[6] * m2.num[9] + m1.num[7] * m2.num[13],
		m1.num[4] * m2.num[2] + m1.num[5] * m2.num[6] + m1.num[6] * m2.num[10] + m1.num[7] * m2.num[14],
		m1.num[4] * m2.num[3] + m1.num[5] * m2.num[7] + m1.num[6] * m2.num[11] + m1.num[7] * m2.num[15],

		m1.num[8] * m2.num[0] + m1.num[9] * m2.num[4] + m1.num[10] * m2.num[8] + m1.num[11] * m2.num[12],
		m1.num[8] * m2.num[1] + m1.num[9] * m2.num[5] + m1.num[10] * m2.num[9] + m1.num[11] * m2.num[13],
		m1.num[8] * m2.num[2] + m1.num[9] * m2.num[6] + m1.num[10] * m2.num[10] + m1.num[11] * m2.num[14],
		m1.num[8] * m2.num[3] + m1.num[9] * m2.num[7] + m1.num[10] * m2.num[11] + m1.num[11] * m2.num[15],

		m1.num[12] * m2.num[0] + m1.num[13] * m2.num[4] + m1.num[14] * m2.num[8] + m1.num[15] * m2.num[12],
		m1.num[12] * m2.num[1] + m1.num[13] * m2.num[5] + m1.num[14] * m2.num[9] + m1.num[15] * m2.num[13],
		m1.num[12] * m2.num[2] + m1.num[13] * m2.num[6] + m1.num[14] * m2.num[10] + m1.num[15] * m2.num[14],
		m1.num[12] * m2.num[3] + m1.num[13] * m2.num[7] + m1.num[14] * m2.num[11] + m1.num[15] * m2.num[15],
	};
}


Matrix4f::Matrix4f(std::initializer_list<float> il)
{
	std::copy(il.begin(), il.end(), std::begin(num));
}

Vector4f operator*(const Matrix4f& m, const Vector4f& v)
{
	return Vector4f{
		m.num[0] * v.x + m.num[1] * v.y + m.num[2] * v.z + m.num[3] * v.w,
		m.num[4] * v.x + m.num[5] * v.y + m.num[6] * v.z + m.num[7] * v.w,
		m.num[8] * v.x + m.num[9] * v.y + m.num[10] * v.z + m.num[11] * v.w,
		m.num[12] * v.x + m.num[13] * v.y + m.num[14] * v.z + m.num[15] * v.w,
	};
}

Matrix4f MathUtility::getPerspctiveMatrix(float fov, float aspectRatio, float zNear, float zFar)
{
	float n = -zNear;
	float f = -zFar;
	float t = std::abs(n) * std::tan(fov / 2);
	float b = -t;
	float r = t * aspectRatio;
	float l = -r;

	return Matrix4f{
		2 * n / (r - l), 0, (l + r) / (l - r), 0,
		0, 2 * n / (t - b), (b + t) / (b - t), 0,
		0, 0, (n + f) / (n - f), 2 * n * f / (f - n),
		0, 0, 1, 0,
	};
}

Matrix4f MathUtility::rotateX(const float angle)
{
	auto radians = angle * PI / 180.f;
	return Matrix4f{
		1, 0, 0, 0,
		0, std::cos(radians), -std::sin(radians), 0,
		0, std::sin(radians), std::cos(radians), 0,
		0, 0, 0, 1,
	};
}

Matrix4f MathUtility::rotateY(const float angle)
{
	auto radians = angle * PI / 180.f;
	return Matrix4f{
		std::cos(radians), 0, std::sin(radians), 0,
		0, 1, 0, 0,
		-std::sin(radians), 0, std::cos(radians), 0,
		0, 0, 0, 1
	};
}

Matrix4f MathUtility::rotateZ(const float angle)
{
	auto radians = angle * PI / 180.f;
	return Matrix4f{
		std::cos(radians), -std::sin(radians), 0, 0,
		std::sin(radians), std::cos(radians), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
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
		4, 5, 7,
		7, 8, 7 
	};

	Matrix4f m2 = {
		1, 2, 3, 4,
		5, 6, 7, 1,
		9, 10 , 6, 1,
		13, 4, 15, 1
	};

	Matrix3f m3 = {
		1, 7, 8,
		2, 6, 3,
		9, 6, 2
	};

	Matrix4f m4 = {
		1, 9, 7, 3,
		9, 6, 3, 4,
		25, 6, 1, 4,
		7, 9, 17, 23
	};

	t = m1 * v1;
	std::cout << "m1 multiply v1: " << t.x << " " << t.y << " " << t.z << std::endl;

	t2 = m2 * v3;
	std::cout << "m2 multiply v3 : " << t2.x << " " << t2.y << " " << t2.z << " " << t2.w << std::endl;

	std::cout << "det m1 : " << m1.determinant() << std::endl;
	std::cout << "det m2 : " << m2.determinant() << std::endl;

	Matrix3f t3 = m1.transpose();
	std::cout << "m1.transpose() : " << std::endl
		<< t3.num[0] << " " << t3.num[1] << " " << t3.num[2] << std::endl
		<< t3.num[3] << " " << t3.num[4] << " " << t3.num[5] << std::endl
		<< t3.num[6] << " " << t3.num[7] << " " << t3.num[8] << std::endl;

	t3 = m1.inverse();
	std::cout << "m1.inverse() : " << std::endl
		<< t3.num[0] << " " << t3.num[1] << " " << t3.num[2] << std::endl
		<< t3.num[3] << " " << t3.num[4] << " " << t3.num[5] << std::endl
		<< t3.num[6] << " " << t3.num[7] << " " << t3.num[8] << std::endl;

	t3 = m1 * m3;
	std::cout << "m1 * m3 : " << std::endl
		<< t3.num[0] << " " << t3.num[1] << " " << t3.num[2] << std::endl
		<< t3.num[3] << " " << t3.num[4] << " " << t3.num[5] << std::endl
		<< t3.num[6] << " " << t3.num[7] << " " << t3.num[8] << std::endl;

	Matrix4f t4 = m2.transpose();
	std::cout << "m2.transpose() : " << std::endl
		<< t4.num[0] << " " << t4.num[1] << " " << t4.num[2] << " " << t4.num[3] << std::endl
		<< t4.num[4] << " " << t4.num[5] << " " << t4.num[6] << " " << t4.num[7] << std::endl
		<< t4.num[8] << " " << t4.num[9] << " " << t4.num[10]<< " " << t4.num[11] << std::endl
		<< t4.num[12]<< " " << t4.num[13]<< " " << t4.num[14]<< " " << t4.num[15] << std::endl;

	t4 = m2.inverse();
	std::cout << "m2.inverse() : " << std::endl
		<< t4.num[0] << " " << t4.num[1] << " " << t4.num[2] << " " << t4.num[3] << std::endl
		<< t4.num[4] << " " << t4.num[5] << " " << t4.num[6] << " " << t4.num[7] << std::endl
		<< t4.num[8] << " " << t4.num[9] << " " << t4.num[10] << " " << t4.num[11] << std::endl
		<< t4.num[12] << " " << t4.num[13] << " " << t4.num[14] << " " << t4.num[15] << std::endl;

	t4 = m2 * m4;
	std::cout << "m2 * m4 : " << std::endl
		<< t4.num[0] << " " << t4.num[1] << " " << t4.num[2] << " " << t4.num[3] << std::endl
		<< t4.num[4] << " " << t4.num[5] << " " << t4.num[6] << " " << t4.num[7] << std::endl
		<< t4.num[8] << " " << t4.num[9] << " " << t4.num[10] << " " << t4.num[11] << std::endl
		<< t4.num[12] << " " << t4.num[13] << " " << t4.num[14] << " " << t4.num[15] << std::endl;
}

#endif