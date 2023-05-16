#include "Camera.h"

Matrix4f Camera::getViewMatrix()
{
	auto transM = Matrix4f{
		1, 0, 0, -Pos.x,
		0, 1, 0, -Pos.y,
		0, 0, 1, -Pos.z,
		0, 0, 0, 1
	};

	auto Ryaw = MathUtility::rotateY(yaw);
	auto Rpitch = MathUtility::rotateX(pitch);
	auto Rroll = MathUtility::rotateZ(roll);

	auto M = Ryaw * Rpitch * Rroll;
	auto newFwd = M * fwdDirection;
	auto newUp = M * upDirection;

	auto t = static_cast<Vector3f>(newFwd).crossProduct(static_cast<Vector3f>(newUp));
	auto newRight = static_cast<Vector4f>(t);

	auto rotateM = Matrix4f{
		newRight.x, newRight.y, newRight.z, 0,
		newUp.x, newUp.y, newUp.z, 0,
		-newFwd.x, -newFwd.y, -newFwd.z, 0,
		0, 0, 0, 1
	};

	return rotateM * transM;
}

void Camera::addYaw(const float angle)
{
	this->yaw += angle;
}
void Camera::addPitch(const float angle)
{
	this->pitch = MathUtility::clamp(this->pitch + angle, -89.9f, 89.9f);
}

void Camera::MoveFwd(const float dis)
{
	if (fabs(dis) < 1e-6)
		return;

	auto Ryaw = MathUtility::rotateY(yaw);
	auto Rpitch = MathUtility::rotateX(pitch);
	auto Rroll = MathUtility::rotateZ(roll);

	auto newFwd = Ryaw * Rpitch * Rroll * fwdDirection;
	Pos = Pos + dis * static_cast<Vector3f>(newFwd);
}

void Camera::MoveRight(const float dis)
{
	if (fabs(dis) < 1e-6)
		return;
	auto Ryaw = MathUtility::rotateY(yaw);
	auto Rpitch = MathUtility::rotateX(pitch);
	auto Rroll = MathUtility::rotateZ(roll);

	auto M = Ryaw * Rpitch * Rroll;
	auto newFwd = M * fwdDirection;
	auto newUp = M * upDirection;

	auto t = static_cast<Vector3f>(newFwd).crossProduct(static_cast<Vector3f>(newUp));

	Pos = Pos + dis * t;
}

void Camera::MoveUp(const float dis)
{
	Pos = Pos + dis * Vector3f(0, 1, 0);
}