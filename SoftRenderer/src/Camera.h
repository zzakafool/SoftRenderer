#ifndef M_CAMERA_H
#define M_CAMERA_H

#include "Math.h"

class Camera
{
public:
	// in angles
	float roll = 0;
	float yaw = 0;
	float pitch = 0;
	Vector3f Pos = {0, 0, 10};

	// initial direction in world-coord
	// face -z, up +y
	Vector4f upDirection = { 0, 1, 0, 0 };
	Vector4f fwdDirection = { 0, 0, -1, 0 };

	Matrix4f getViewMatrix();
	void addYaw(const float angle);
	void addPitch(const float angle);
	void MoveFwd(const float dis);
	void MoveRight(const float dis);
	void MoveUp(const float dis);
};

#endif