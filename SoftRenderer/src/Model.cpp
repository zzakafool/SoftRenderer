#include "Model.h"

#include <iostream>
#include <string>
#include <fstream>
#include <queue>

using namespace std;

void Model::load(const string &path)
{
	unsigned int assimp_read_flag = aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes |
		aiProcess_ValidateDataStructure |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FlipWindingOrder;
	// the property must be false, for fixing some mixamo-anim problem.
	import.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	scene = import.ReadFile(path, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode);
}

void processAnim(aiAnimation* anim, Mesh* mesh)
{
	for (int j = 0; j < anim->mNumChannels; ++j)
	{
		auto t = anim->mChannels[j];
		std::cout << t->mNumPositionKeys << std::endl;
	}
}

void Model::processNode(aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh));
	}

	for (int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i]);
	}
}

void Mesh::addToRenderer(Renderer* render)
{
	posbufId = render->addPositionBuf(std::move(positions));
	norbufId = render->addNormalBuf(std::move(normals));
	uvbufId = render->addUVBuf(std::move(uvCoords));
	indbufId = render->addIndexBuf(std::move(indices));
	boneWeightBufId = render->addBoneWeightBuf(std::move(boneWeight));
}
void Mesh::setDrawParams(DrawParams& dp, float timeInSecs)
{
	dp.posId = posbufId;
	dp.norId = norbufId;
	dp.uvId = uvbufId;
	dp.indId = indbufId;
	dp.boneWeightId = boneWeightBufId;

	dp.fsParams.Ka = material.Ka;
	dp.fsParams.Kd = material.Kd;
	dp.fsParams.Ks = material.Ks;
	dp.fsParams.Ns = material.Ns;

	dp.fsParams.diffuseTextureIdx = this->diffuseTextureIdx;
	dp.fsParams.specularTextureIdx = this->specularTextureIdx;
	
	if(this->anim != nullptr)
		getBoneTransform(timeInSecs, dp.boneTransform);
}

Mesh Model::processMesh(aiMesh* mesh)
{
	Mesh res;
	res.scene = scene;

	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vector3f pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		res.positions.push_back(pos);

		Vector3f normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		res.normals.push_back(normal);

		Vector2f uvCoord;
		uvCoord.x = mesh->mTextureCoords[0][i].x;
		uvCoord.y = mesh->mTextureCoords[0][i].y;
		res.uvCoords.push_back(uvCoord);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);

		res.indices.push_back(Vector3i{ static_cast<int>(face.mIndices[0]), static_cast<int>(face.mIndices[1]), static_cast<int>(face.mIndices[2])});
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		aiColor3D ka;
		aiColor3D kd;
		aiColor3D ks;
		float shine = 0.0;
		material->Get(AI_MATKEY_COLOR_AMBIENT, ka);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
		material->Get(AI_MATKEY_COLOR_SPECULAR, ks);
		material->Get(AI_MATKEY_SHININESS, shine);

		processTexture(aiTextureType_DIFFUSE, &res, material);
		processTexture(aiTextureType_SPECULAR, &res, material);

		Vector3f tka{ ka.r, ka.g, ka.b };
		Vector3f tkd{ kd.r,kd.g,kd.b };
		Vector3f tks{ ks.r, ks.g, ks.b };
		
		res.material = { tka, tkd, tks, shine };
	}

	// has bones and animations
	if (mesh->HasBones())
	{
		res.boneWeight.resize(res.positions.size());
		auto n = mesh->mNumBones;
		for (int i = 0; i < n; ++i)
		{
			auto t = mesh->mBones[i];
			std::string boneName(t->mName.C_Str());
			//std::cout << "bone Name : " << boneName << std::endl;

			auto it = res.boneMap.find(boneName);
			int idx = -1;
			if (it == res.boneMap.end())
			{
				auto mat = mesh->mBones[i]->mOffsetMatrix;
				Bone b;
				b.offsetMatrix = {
					mat.a1, mat.a2, mat.a3, mat.a4,
					mat.b1, mat.b2, mat.b3, mat.b4,
					mat.c1, mat.c2, mat.c3, mat.c4,
					mat.d1, mat.d2, mat.d3, mat.d4,
				};

				res.boneVec.push_back(std::move(b));
				idx = res.boneVec.size() - 1;
				res.boneMap.insert({ boneName, idx});
			}
			else
			{
				idx = it->second;
			}

			for (int j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
			{
				auto vertexid = mesh->mBones[i]->mWeights[j].mVertexId;
				auto weight = mesh->mBones[i]->mWeights[j].mWeight;

				res.boneWeight[vertexid].push_back(make_pair(idx, weight));
			}
		}
		
		// if has Bones, we assume this scene has just *only an* anim, which could be used in this mesh
		if (scene->HasAnimations())
		{
			assert(scene->mNumAnimations <= 1);
			auto anim = scene->mAnimations[0];
			res.anim = anim;

			//for (int i = 0; i < anim->mNumChannels; ++i)
			//{
			//	std::cout << anim->mChannels[i]->mNodeName.C_Str() << std::endl;
			//}

			//std::cout << std::endl << scene->mRootNode->mName.C_Str() << std::endl;
			//for (int k = 0; k < scene->mRootNode->mNumChildren; ++k)
			//{
			//	std::cout << scene->mRootNode->mChildren[k]->mName.C_Str() << std::endl;
			//}

			//auto& t = scene->mRootNode->mTransformation;
			//res.m_GlobalInverseTransform = Matrix4f{
			//	t.a1, t.a2, t.a3, t.a4,
			//	t.b1, t.b2, t.b3, t.b4,
			//	t.c1, t.c2, t.c3, t.c4,
			//	t.d1, t.d2, t.d3, t.d4
			//}.inverse();
		}
	}

	return res;
}

void Mesh::getBoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms)
{
	if (anim == nullptr)
		return;
	float TicksPerSecond = anim->mTicksPerSecond;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = std::fmodf(TimeInTicks, anim->mDuration);

	Matrix4f identity = Matrix4f::Identity();
	ReadNodeHierarchy(AnimationTime, scene->mRootNode, identity);
	
	Transforms.resize(this->boneVec.size());

	// -------------------
	for (int i = 0; i < this->boneVec.size(); ++i)
	{
		Transforms[i] = boneVec[i].finalTransformation;
	}
}

void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
	// pNode might be not a bone node
	string NodeName(pNode->mName.data);

	auto& t = pNode->mTransformation;
	Matrix4f NodeTransformation = {
		t.a1, t.a2, t.a3, t.a4,
		t.b1, t.b2, t.b3, t.b4,
		t.c1, t.c2, t.c3, t.c4,
		t.d1, t.d2, t.d3, t.d4,
	};

	const aiNodeAnim* pNodeAnim = FindNodeAnim(anim, pNode->mName.C_Str());

	if (pNodeAnim && boneMap.find(NodeName) != boneMap.end())
	{
		// interpolate scaling
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix4f ScalingM = {
			Scaling.x, 0, 0, 0,
			0, Scaling.y, 0, 0,
			0, 0, Scaling.z, 0,
			0, 0, 0, 1
		};

		// interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		auto t = RotationQ.GetMatrix();
		Matrix4f RotationM = Matrix4f{
			t.a1, t.a2, t.a3, 0,
			t.b1, t.b2, t.b3, 0,
			t.c1, t.c2, t.c3, 0,
			0, 0, 0, 1
		};

		// interpolate translation
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		Matrix4f TranslationM = {
			1, 0, 0, Translation.x,
			0, 1, 0, Translation.y,
			0, 0, 1, Translation.z,
			0, 0, 0, 1
		};

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) 
	{
		auto boneid = boneMap[NodeName];
		boneVec[boneid].finalTransformation = /*m_GlobalInverseTransform **/ GlobalTransformation * boneVec[boneid].offsetMatrix;
	}

	for (int i = 0; i < pNode->mNumChildren; ++i)
	{
		ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}

}

void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	// find the lowerbound idx of AnimationTime.
	int lower_idx = 0;
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; ++i)
	{
		float t = static_cast<float>(pNodeAnim->mPositionKeys[i + 1].mTime);

		if (AnimationTimeTicks < t)
		{
			lower_idx = i;
			break;
		}
	}

	int upper_idx = lower_idx + 1;
	assert(upper_idx < pNodeAnim->mNumPositionKeys);
	float t1 = static_cast<float>(pNodeAnim->mPositionKeys[lower_idx].mTime);
	float t2 = static_cast<float>(pNodeAnim->mPositionKeys[upper_idx].mTime);

	float deltaTime = t2 - t1;
	float factor = (AnimationTimeTicks - t1) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	const aiVector3D& start = pNodeAnim->mPositionKeys[lower_idx].mValue;
	const aiVector3D& end = pNodeAnim->mPositionKeys[upper_idx].mValue;
	aiVector3D delta = end - start;
	Out = start + factor * delta;
}

void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTick, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	// find the lowerbound idx of AnimationTime.
	int lower_idx = 0;
	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; ++i)
	{
		float t = static_cast<float>(pNodeAnim->mRotationKeys[i + 1].mTime);
		if (AnimationTimeTick < t)
		{
			lower_idx = i;
			break;
		}
	}

	int upper_idx = lower_idx + 1;
	assert(upper_idx < pNodeAnim->mNumRotationKeys);

	// caculate the time between [lower, upper]
	float deltaTime = pNodeAnim->mRotationKeys[upper_idx].mTime - pNodeAnim->mRotationKeys[lower_idx].mTime;
	float factor = (AnimationTimeTick - static_cast<float>(pNodeAnim->mRotationKeys[lower_idx].mTime)) / deltaTime;
	
	assert(factor >= 0.0f && factor <= 1.0f);

	const aiQuaternion& startRotationQ = pNodeAnim->mRotationKeys[lower_idx].mValue;
	const aiQuaternion& endRotationQ = pNodeAnim->mRotationKeys[upper_idx].mValue;
	aiQuaternion::Interpolate(Out, startRotationQ, endRotationQ, factor);
	Out = Out.Normalize();
}

void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	// find the lowerbound idx of AnimationTime.
	int lower_idx = 0;
	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; ++i)
	{
		float t = static_cast<float>(pNodeAnim->mScalingKeys[i + 1].mTime);

		if (AnimationTimeTicks < t)
		{
			lower_idx = i;
			break;
		}
	}

	int upper_idx = lower_idx + 1;
	assert(upper_idx < pNodeAnim->mNumScalingKeys);
	float t1 = static_cast<float>(pNodeAnim->mScalingKeys[lower_idx].mTime);
	float t2 = static_cast<float>(pNodeAnim->mScalingKeys[upper_idx].mTime);

	float deltaTime = t2 - t1;
	float factor = (AnimationTimeTicks - t1) / deltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);
	const aiVector3D& start = pNodeAnim->mScalingKeys[lower_idx].mValue;
	const aiVector3D& end = pNodeAnim->mScalingKeys[upper_idx].mValue;
	aiVector3D delta = end - start;
	Out = start + factor * delta;
}


aiNodeAnim* Mesh::FindNodeAnim(aiAnimation* pAnim, const std::string& nodeName)
{
	for (int i = 0; i < pAnim->mNumChannels; ++i)
	{
		auto& p = pAnim->mChannels[i];
		if (nodeName == std::string(p->mNodeName.C_Str()))
		{
			return p;
		}
	}

	return nullptr;
}

aiNode* Mesh::findAnimRootBone()
{
	auto rootNode = scene->mRootNode;
	string name;
	std::queue<aiNode*> que;

	que.push(rootNode);
	while (!que.empty())
	{
		auto t = que.front();
		que.pop();
		name = t->mName.C_Str();
		if (boneMap.find(name) != boneMap.end())
			return t;

		for (int i = 0; i < t->mNumChildren; ++i)
		{
			que.push(t->mChildren[i]);
		}
	}

	return nullptr;
}


void Model::processTexture(aiTextureType type, Mesh* mesh, aiMaterial *material)
{
	auto count = material->GetTextureCount(type);
	if (count == 0)
	{
		if (type == aiTextureType_DIFFUSE)
			mesh->diffuseTextureIdx = -1;
		else if (type == aiTextureType_SPECULAR)
			mesh->specularTextureIdx = -1;
		return;
	}
	for (int i = 0; i < count; ++i)
	{
		aiString tpath;
		material->GetTexture(type, i, &tpath);

		auto allPathS = directory + "/" + tpath.C_Str();

		int idx = -1;
		auto it = textureMap.find(allPathS);
		if (it == textureMap.end())
		{
			ifstream f(allPathS.c_str());
			// if file not exist, it might to be a embedded texture, read it from memory
			if (f.good())
			{
				auto newTexture = make_shared<Texture>(allPathS.c_str());
				textureVec.push_back(newTexture);
				idx = textureVec.size() - 1;
				textureMap.insert({ allPathS, idx });
			}
			else
			{
				auto texture = scene->GetEmbeddedTexture(allPathS.c_str());
				// If this value is zero, pcData points to an compressed texture in any format (e.g. JPEG).
				if (texture->mHeight == 0)
				{
					auto stream = SDL_RWFromMem(static_cast<void*>(texture->pcData), texture->mWidth);
					if (stream == NULL)
					{
						std::cout << "read texture from memory error." << std::endl;
					}
					auto newTexture = make_shared<Texture>(stream);
					if (SDL_RWclose(stream) < 0)
					{
						std::cout << "close sdl_rw error." << std::endl;
					}

					textureVec.push_back(newTexture);
					idx = textureVec.size() - 1;
					textureMap.insert({ allPathS, idx });
				}
				else
				{
					std::cout << "not a compressed texture." << std::endl;
				}
			}
		}
		else
		{
			idx = it->second;
		}

		if (type == aiTextureType_DIFFUSE)
			mesh->diffuseTextureIdx = idx;
		else if (type == aiTextureType_SPECULAR)
			mesh->specularTextureIdx = idx;
	}
}