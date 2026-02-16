#pragma once

#include <unordered_map>

#include <d3d11.h>
#include <DirectXMath.h>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

#include "collision.h"

struct MODEL
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	AABB local_aabb;
};


MODEL* ModelLoad(const char* FileName, float scale, bool bBlender);
void ModelDraw(MODEL* model, const DirectX::XMMATRIX& mtxWorld);
void ModelUnlitDraw(MODEL* model, const DirectX::XMMATRIX& mtxWorld);
void ModelRelease(MODEL* model);

AABB Model_GetAABB(MODEL* model, const DirectX::XMFLOAT3& position);
