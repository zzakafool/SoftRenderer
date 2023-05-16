#include "Renderer.h"

Renderer::Renderer(SDL_Surface* src) : renderTexture(src), zBuf(src->w * src->h, 0.f)
{

}

void Renderer::clearColor(const Vector4f &col)
{
	renderTexture.clear(col);
}
void Renderer::clearZ()
{
	std::fill(zBuf.begin(), zBuf.end(), 0.f);
}

void Renderer::draw(DrawParams &param)
{
	if (param.type == Primitive::Point)
		drawPoint(param);
	else
		drawTriangle(param);
}

void Renderer::drawPoint(DrawParams param)
{
	VertexShaderParams& vsp = param.vsParams;
	auto posbuf = posBufs.at(param.posId.id);
	auto& boneWeightBuf = boneWeightBufs.at(param.boneWeightId.id);

	for (int i = 0; i < posbuf.size(); ++i)
	{
		vsp.pos = static_cast<Vector4f>(posbuf[i]);
		vsp.pos.w = 1;

		vsp.allBonesTransform = Matrix4f::Identity();
		if (boneWeightBuf.size() > 0)
		{
			// calculate allBoneTransform
			Matrix4f transform = Matrix4f::Zero();
			auto& posBoneWeights = boneWeightBuf[i];
			for (const auto& pairW : posBoneWeights)
			{
				auto boneId = pairW.first;
				auto weight = pairW.second;
				transform = transform + (weight * param.boneTransform[boneId]);
			}
			vsp.allBonesTransform = transform;
		}

		auto homoPos = pfVertexShader(vsp);
		homoPos = (1.f / homoPos.w) * homoPos;

		int ti = (homoPos.x + 1.0) / 2 * renderTexture.width;
		int tj = (homoPos.y + 1.0) / 2 * renderTexture.height;

		setColor(ti, tj, Vector4f{ 255, 0, 0, 0 });
	}

	
}

bool Renderer::isInside(float x, float y, const Vector4f* triPos)
{
	auto sign = [](float num) -> int {
		if (num < 0)
			return -1;
		else if (num == 0)
			return 0;
		return 1;
	};

	Vector3f a = Vector3f{ triPos[0].x, triPos[0].y, 0 };
	Vector3f b = Vector3f{ triPos[1].x, triPos[1].y, 0 };
	Vector3f c = Vector3f{ triPos[2].x, triPos[2].y, 0 };
	Vector3f p = { x, y, 0 };

	Vector3f ab = b - a;
	Vector3f bc = c - b;
	Vector3f ca = a - c;

	Vector3f ap = p - a;
	Vector3f bp = p - b;
	Vector3f cp = p - c;

	return sign(ab.x * ap.y - ap.x * ab.y) == sign(bc.x * bp.y - bp.x * bc.y) &&
		sign(bc.x * bp.y - bp.x * bc.y) == sign(ca.x * cp.y - cp.x * ca.y);
}

Vector3f Renderer::getBarycentricCoord(float x, float y, const Vector4f* triPos)
{
	Vector3f a = Vector3f{ triPos[0].x, triPos[0].y, 0};
	Vector3f b = Vector3f{ triPos[1].x, triPos[1].y, 0 };
	Vector3f c = Vector3f{ triPos[2].x, triPos[2].y, 0 };
	Vector3f p = { x, y, 0 };

	Vector3f ab = b - a;
	Vector3f bc = c - b;
	Vector3f ca = a - c;

	Vector3f ap = p - a;
	Vector3f bp = p - b;
	Vector3f cp = p - c;

	Vector3f res;
	res.x = (bp.crossProduct(bc)).z / (-ab).crossProduct(bc).z;
	res.y = (cp.crossProduct(ca)).z / (-bc).crossProduct(ca).z;
	res.z = (ap.crossProduct(ab)).z / (-ca).crossProduct(ab).z;

	return res;
}

void Renderer::drawTriangle(DrawParams param)
{
	VertexShaderParams& vsp = param.vsParams;
	FragmentShaderParams& fsp = param.fsParams;

	auto &posbuf = posBufs.at(param.posId.id);
	auto &indbuf = indBufs.at(param.indId.id);
	//auto colbuf = colorBufs.at(param.colId.id);
	auto &uvbuf = uvBufs.at(param.uvId.id);
	auto &norbuf = normalBufs.at(param.norId.id);
	auto &boneWeightBuf = boneWeightBufs.at(param.boneWeightId.id);

	std::vector<Vector4f> portPosBuf;
	std::vector<Vector4f> viewPosBuf;
	std::vector<Vector3f> viewNormalBuf;
	std::vector<bool> isBack;
	portPosBuf.reserve(posbuf.size());
	viewPosBuf.reserve(posbuf.size());
	viewNormalBuf.reserve(posbuf.size());

	float f = vsp.zFar;
	float n = vsp.zNear;
	float p1 = (f - n) / 2;
	float p2 = (f + n) / 2;

	for (int i = 0;i < posbuf.size(); ++i)
	{
		vsp.pos = static_cast<Vector4f>(posbuf[i]);
		vsp.pos.w = 1;
		vsp.pointNormal = norbuf[i];

		// calculate allBoneTransform
		Matrix4f transform = Matrix4f::Zero();

		vsp.allBonesTransform = Matrix4f::Identity();
		if (boneWeightBuf.size() > 0)
		{
			auto& posBoneWeights = boneWeightBuf[i];
			for (const auto& pairW : posBoneWeights)
			{
				auto boneId = pairW.first;
				auto weight = pairW.second;
				transform = transform + (weight * param.boneTransform[boneId]);
			}
			vsp.allBonesTransform = transform;
		}

		// mvp
		auto homoPos = pfVertexShader(vsp);
		
		// divide w
		homoPos.x = (1.f / homoPos.w) * homoPos.x;
		homoPos.y = (1.f / homoPos.w) * homoPos.y;
		homoPos.z = (1.f / homoPos.w) * homoPos.z;

		homoPos.x = (homoPos.x + 1.0) / 2 * renderTexture.width;
		homoPos.y = (homoPos.y + 1.0) / 2 * renderTexture.height;
		homoPos.z = homoPos.z * p1 + p2;

		portPosBuf.push_back(homoPos);
		viewPosBuf.push_back(vsp.viewPos);
		viewNormalBuf.push_back(vsp.pointNormal);
	}

	for (auto it = indbuf.begin(); it != indbuf.end(); ++it)
	{
		int i[] = { it->x, it->y, it->z };
		Vector4f viewPos[] = { viewPosBuf[i[0]], viewPosBuf[i[1]], viewPosBuf[i[2]] };
		if (isBackFace(viewPos))
			continue;
		Vector4f portPos[] = { portPosBuf[i[0]], portPosBuf[i[1]], portPosBuf[i[2]] };
		//Vector4f color[] = { colbuf[i[0]], colbuf[i[1]], colbuf[i[2]] };
		Vector2f uv[] = { uvbuf[i[0]], uvbuf[i[1]], uvbuf[i[2]] };
		Vector3f normals[] = { viewNormalBuf[i[0]].normalize(), viewNormalBuf[i[1]].normalize(), viewNormalBuf[i[2]].normalize()};

		float xMin = portPos[0].x;
		float xMax = portPos[0].x;
		float yMin = portPos[0].y;
		float yMax = portPos[0].y;

		for (const auto& v : portPos)
		{
			xMin = std::min(xMin, v.x);
			xMax = std::max(xMax, v.x);
			yMin = std::min(yMin, v.y);
			yMax = std::max(yMax, v.y);
		}
		
		xMin = std::max(0.f, xMin);
		xMax = std::min(static_cast<float>(renderTexture.width - 1), xMax);
		yMin = std::max(0.f, yMin);
		yMax = std::min(static_cast<float>(renderTexture.height - 1), yMax);

		for (int i = xMin; i <= xMax; ++i)
		{
			for (int j = yMin; j <= yMax; ++j)
			{
				float x = i + 0.5f;
				float y = j + 0.5f;

				if (isInside(x, y, portPos))
				{
					auto barycentricCoord = getBarycentricCoord(x, y, portPos);
					auto alpha = barycentricCoord.x;
					auto beta = barycentricCoord.y;
					auto gamma = barycentricCoord.z;

					auto z_i = alpha * portPos[0].z + beta * portPos[1].z + gamma * portPos[2].z;

					if (z_i > zBuf[getIndex(i, j)])
					{
						zBuf[getIndex(i, j)] = z_i;
						// auto col_i = MathUtility::interpolateByBaryCentric(color, portPos, alpha, beta, gamma);
						auto uv_i = MathUtility::interpolateByBaryCentric(uv, portPos, alpha, beta, gamma);
						auto normal_i = MathUtility::interpolateByBaryCentric(normals, portPos, alpha, beta, gamma).normalize();
						auto viewPos_i = MathUtility::interpolateByBaryCentric(viewPos, portPos, alpha, beta, gamma);
						// fsp.color = col_i;
						fsp.uv = uv_i;
						fsp.normal = normal_i;
						fsp.viewPos = viewPos_i;

						auto fcol = pfFragmentShader(fsp);
						setColor(i, j, fcol);
					}
				}
			}
		}


	}
}

bool Renderer::isBackFace(const Vector4f* triPos)
{
	auto a = static_cast<Vector3f>(triPos[0]);
	auto b = static_cast<Vector3f>(triPos[1]);
	auto c = static_cast<Vector3f>(triPos[2]);

	auto ab = b - a;
	auto bc = c - b;

	return ab.crossProduct(bc).dotProduct(Vector3f{ 0, 0, 1 }) < -0.01f;		// a little magic number for fitting float-precision probrem.
}

// from left-bottom, line first
int Renderer::getIndex(int x, int y)
{
	return y * renderTexture.width + x;
}

// from left-bottom, line first
void Renderer::setColor(int x, int y, const Vector4f& col)
{
	// map to left-top texture coord
	y = renderTexture.height - 1 - y;
	renderTexture.setColor(x, y, col);
}

pos_buf_id Renderer::addPositionBuf(std::vector<Vector3f>&& posBuf)
{
	int id = getNextId();
	posBufs.insert({ id, std::move(posBuf) });
	return { id };
}

ind_buf_id Renderer::addIndexBuf(std::vector<Vector3i>&& indBuf)
{
	int id = getNextId();
	indBufs.insert({ id, std::move(indBuf) });
	return { id };
}

col_buf_id Renderer::addColorBuf(std::vector<Vector4f>&& colorBuf)
{
	int id = getNextId();
	colorBufs.insert({ id, std::move(colorBuf) });
	return { id };
}

nor_buf_id Renderer::addNormalBuf(std::vector<Vector3f>&& normalBuf)
{
	int id = getNextId();
	normalBufs.insert({ id, std::move(normalBuf) });
	return { id };
}

uv_buf_id Renderer::addUVBuf(std::vector<Vector2f>&& uvBuf)
{
	int id = getNextId();
	uvBufs.insert({ id, std::move(uvBuf) });
	return { id };
}

bone_weight_buf_id Renderer::addBoneWeightBuf(std::vector<std::vector<std::pair<int, float>>>&& boneWeightBuf)
{
	int id = getNextId();
	boneWeightBufs.insert({ id, std::move(boneWeightBuf) });
	return { id };
}

void Renderer::setVertexShader(std::function<Vector4f(VertexShaderParams&)> vs)
{
	this->pfVertexShader = vs;
}
void Renderer::setFragmentShader(std::function<Vector4f(FragmentShaderParams&)> fs)
{
	this->pfFragmentShader = fs;
}