#include "fragmentShader.h"

Vector4f fragmentShader(FragmentShaderParams& param)
{
	auto uv = param.uv;
	// auto t = param.diffuseTexture.getColorFromUV(uv.x, uv.y);
	/*return param.color;*/

	Vector3f col = { 0, 0, 0 };

	//auto diffcol = static_cast<Vector3f>(param.diffuseTexture.getColorFromUV(uv.x, uv.y));
	Vector3f La = param.Ka.mulByVector(Vector3f{ 10, 10, 10 });
	//param.Kd = Vector3f{125, 125, 125} / 255.f;
	//param.Ks = Vector3f(0.7937, 0.7937, 0.7937);
	//param.Ns = 150;

	if (uv.x < 0.0f)
		uv.x += 1.0f;
	else if (uv.x >= 1.0f)
		uv.x -= 1.0f;

	if (uv.y < 0.0f)
		uv.y += 1.0f;
	else if (uv.y >= 1.0f)
		uv.x -= 1.0f;

	if (param.diffuseTextureIdx != -1)
	{
		param.Kd = static_cast<Vector3f>(param.textureVec[param.diffuseTextureIdx]->getColorFromUV(uv.x, uv.y)) / 255.f;
	}

	if (param.specularTextureIdx != -1)
	{
		param.Ks = static_cast<Vector3f>(param.textureVec[param.specularTextureIdx]->getColorFromUV(uv.x, uv.y)) / 255.f;
	}

	auto viewPos3 = static_cast<Vector3f>(param.viewPos);
	for (int i = 0; i < param.lights.size(); ++i)
	{
		auto view = (Vector3f{ 0, 0, 0 } - viewPos3).normalize();
		auto light = (param.lights[i].position - viewPos3);
		auto r_2 = light.squreLen();
		auto I_r2 = param.lights[i].intensity / r_2;

		light = light.normalize();
		auto h = (view + light).normalize();
		
		auto Ld = std::max(0.f, param.normal.dotProduct(light)) * param.Kd.mulByVector(I_r2);
		auto Ls = std::powf(std::max(0.f, param.normal.dotProduct(h)), param.Ns) * param.Ks.mulByVector(I_r2);

		col = col + La + Ld + Ls;
	}

	return static_cast<Vector4f>(255 * col);
}