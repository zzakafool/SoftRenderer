#ifndef M_RENDERER_H
#define M_RENDERER_H

#include "Texture.h"
#include "Math.h"
#include <map>
#include <vector>
#include <functional>
//#include "Model.h"
#include "Light.h"

struct VertexShaderParams
{
	// in
	Matrix4f mv;
	Matrix4f p;
	Matrix4f mv_i_T;	// (MV).inverse().transpose();
	Vector4f pos;

	float zNear;
	float zFar;

	//in for anim
	Matrix4f allBonesTransform;

	// out
	Vector4f viewPos;

	//in-out 
	Vector3f pointNormal;
};
struct FragmentShaderParams
{
	Vector4f portPos[3];	// portView coord

	// interpolated
	Vector4f viewPos;
	Vector4f color;
	Vector3f normal;
	Vector2f uv;
	
	// for texture
	std::vector<std::shared_ptr<Texture>> textureVec;
	int diffuseTextureIdx = -1;
	int specularTextureIdx = -1;

	Vector3f Ka;
	Vector3f Kd;
	Vector3f Ks;
	float Ns;
	std::vector<Light> lights;
};

enum class Primitive
{
	Point,
	Triangle,
	MAX_SIZE,
};

struct pos_buf_id
{
	int id = 0;
};

struct ind_buf_id
{
	int id = 0;
};

struct col_buf_id
{
	int id = 0;
};

struct nor_buf_id
{
	int id = 0;
};

struct uv_buf_id
{
	int id = 0;
};

struct bone_weight_buf_id
{
	int id = 0;
};

struct DrawParams
{
	pos_buf_id posId;
	ind_buf_id indId;
	col_buf_id colId;
	nor_buf_id norId;
	uv_buf_id uvId;
	bone_weight_buf_id boneWeightId;

	// anim
	std::vector<Matrix4f> boneTransform;		// bone id -> transform

	VertexShaderParams vsParams;
	FragmentShaderParams fsParams;
	Primitive type;
};

class Renderer
{
private:
	Texture renderTexture;							// renderTexture, framebuf
	std::map<int, std::vector<Vector3f>> posBufs;
	std::map<int, std::vector<Vector3i>> indBufs;
	std::map<int, std::vector<Vector4f>> colorBufs;
	std::map<int, std::vector<Vector3f>> normalBufs;
	std::map<int, std::vector<Vector2f>> uvBufs;

	// bufid -> map{ posId -> {boneid, weight} }
	std::map<int, std::vector<std::vector<std::pair<int, float>>>> boneWeightBufs;

	std::vector<float> zBuf;
	
	std::function<Vector4f(VertexShaderParams&)> pfVertexShader;
	std::function<Vector4f(FragmentShaderParams&)> pfFragmentShader;
	
	int bufId = 1;
	int getNextId() { return bufId++; };	 // from 1 ~
	void drawPoint(DrawParams param);
	void drawTriangle(DrawParams param);

	int getIndex(int x, int y);
	bool isInside(float x, float y, const Vector4f* triPos);
	bool isBackFace(const Vector4f* triPos);
	Vector3f getBarycentricCoord(float x, float y, const Vector4f* triPos);

public:
	Renderer() {};
	Renderer(SDL_Surface *src);
	
	pos_buf_id addPositionBuf(std::vector<Vector3f>&& posBuf);
	ind_buf_id addIndexBuf(std::vector<Vector3i>&& indBuf);
	col_buf_id addColorBuf(std::vector<Vector4f>&& colorBuf);
	nor_buf_id addNormalBuf(std::vector<Vector3f>&& normalBuf);
	uv_buf_id  addUVBuf(std::vector<Vector2f>&& uvBuf);
	bone_weight_buf_id addBoneWeightBuf(std::vector<std::vector<std::pair<int, float>>>&& boneWeightBuf);

	void clearColor(const Vector4f& col);
	void clearZ();

	void setVertexShader(std::function<Vector4f(VertexShaderParams&)>);
	void setFragmentShader(std::function<Vector4f(FragmentShaderParams&)>);

	void setColor(int x, int y, const Vector4f& col);
	void draw(DrawParams &param);
    Texture& getRenderTexture() { return renderTexture; };
};

#endif