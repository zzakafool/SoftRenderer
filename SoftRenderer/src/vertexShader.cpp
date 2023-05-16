#include "vertexShader.h"

Vector4f vectexShader(VertexShaderParams& param)
{
	auto inPos = param.allBonesTransform * param.pos;

	auto t = param.mv * inPos;
	param.viewPos = t;

	// caculate the point normal in view space
	auto tnormal = param.mv_i_T * static_cast<Vector4f>(param.pointNormal);
	param.pointNormal = static_cast<Vector3f>(tnormal);

	return param.p * t;
}