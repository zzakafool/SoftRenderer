#ifndef M_LIGHT_H
#define M_LIGHT_H

#include "Math.h"

// point light
class Light
{
public:
	// just put it in view space simply.
	Vector3f position = {0, 0, 0};
	Vector3f intensity = {500, 500, 500};

	Light(){}
	Light(Vector3f pos, Vector3f it) : position(pos), intensity(it){}
};

#endif