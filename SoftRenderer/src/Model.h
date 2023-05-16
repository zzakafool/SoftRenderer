#ifndef M_MODEL_LOADER_H
#define M_MODEL_LOADER_H

#include "Math.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>
#include "Renderer.h"
#include <unordered_map>
#include <memory>

struct Bone
{
	Matrix4f offsetMatrix;
	Matrix4f finalTransformation;
};

struct Material
{
	Vector3f Ka;
	Vector3f Kd;
	Vector3f Ks;
	float Ns;
};

struct Mesh
{
	Mesh(){}
	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> uvCoords;
	std::vector<Vector3i> indices;

	aiAnimation* anim = nullptr;
	const aiScene* scene = nullptr;
	std::vector<std::vector<std::pair<int, float>>> boneWeight;		// posid -> vec<boneid | weight>;
	std::unordered_map<std::string, int> boneMap;
	std::vector<Bone> boneVec;

	pos_buf_id posbufId;
	nor_buf_id norbufId;
	uv_buf_id uvbufId;
	ind_buf_id indbufId;
	bone_weight_buf_id boneWeightBufId;

	Material material;
	
	int diffuseTextureIdx;
	int specularTextureIdx;

	void addToRenderer(Renderer* render);
	void setDrawParams(DrawParams& dp, float timeInSecs = 0.0f);
	//Matrix4f m_GlobalInverseTransform;

	// the root bone of mixamo-animation is not RootNode( scene-> mRootNode ), in fact it is the mixamorig-Hip
	// so we travel from mRootNode to the leaf, to find the first Bone as the root bone.
	aiNode* findAnimRootBone();
private:
	void getBoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms);
	void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	aiNodeAnim* FindNodeAnim(aiAnimation* pAnim, const std::string &nodeName);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim);


};

class Model
{
public:
	void load(const std::string& path);
	std::vector<Mesh> meshes;
	std::vector<std::shared_ptr<Texture>> textureVec;
	std::unordered_map<std::string, int> textureMap;

private:
	void processNode(aiNode* node);
	void processTexture(aiTextureType type, Mesh* mesh, aiMaterial *material);
	Mesh processMesh(aiMesh* mesh);

	Assimp::Importer import;
	const aiScene* scene;
	std::string directory;
};

#endif