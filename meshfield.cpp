
// ========================================================================
//    Name : mesh field(meshfield.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/19
// ========================================================================

#include "meshfield.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "shader_field.h"
#include "texture.h"
#include "camera.h"
#include "player_camera.h"
#include "player.h"

static constexpr float MESH_SIZE = 1.0f;
static constexpr int MESH_H_COUNT = 50;
static constexpr int MESH_V_COUNT = 50;
static constexpr int MESH_H_VERTEX_COUNT = MESH_H_COUNT + 1;
static constexpr int MESH_V_VERTEX_COUNT = MESH_V_COUNT + 1;
static constexpr int NUM_VERTEX = MESH_H_VERTEX_COUNT * MESH_V_VERTEX_COUNT; // 頂点数
static constexpr int NUM_INDEX = 3 * 2 * MESH_H_COUNT * MESH_V_COUNT; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr;

static int g_MeshFieldTexId0 = -1;
static int g_MeshFieldTexId1 = -1;

static float g_MeshFieldPositionX = 0.0f;
static float g_MeshFieldPositionZ = 0.0f;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static constexpr XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

struct Vertex3D
{
	XMFLOAT3 position;  // 頂点座標
	XMFLOAT3 normal;
	XMFLOAT4 color;		// color
	XMFLOAT2 texcoord;  // UV
};

static Vertex3D g_MeshFieldVertex[NUM_VERTEX] = {

	//前 - z
	//{{-0.5, 0.5,-0.5}, color, {0.0f,0.0f}},		// 0
	//{{ 0.5,-0.5,-0.5}, color, {0.25f,0.25f}},	// 1
	//{{-0.5,-0.5,-0.5}, color, {0.0f,0.25f}},		// 2

	//{{-0.5, 0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f,0.0f}},
	//{{ 0.5, 0.5,-0.5}, color, {0.25f,0.0f}},		// 3
	//{{ 0.5,-0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.25f,0.25f}},

	/*{{-0.5, 0.5, 0.5}, color, {0.0f,0.0f}},
	{{ 0.5, 0.5,-0.5}, color, {0.25f,0.25f}},
	{{-0.5, 0.5,-0.5}, color, {0.0f,0.25f}},
	{{ 0.5, 0.5, 0.5}, color, {0.25f,0.0f}},

	{{ 0.5, 0.5, 0.5}, color, {0.0f,0.0f}},
	{{ 1.5, 0.5,-0.5}, color, {0.25f,0.25f}},
	{{ 0.5, 0.5,-0.5}, color, {0.0f,0.25f}},
	{{ 1.5, 0.5, 0.5}, color, {0.25f,0.0f}},*/
};

static unsigned short g_MeshFieldIndex[NUM_INDEX]{
	/*0,1,2,0,3,1,
	4,5,6,4,7,5,*/
};

void Meshfield_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	g_MeshFieldPositionX = 0.0f;
	g_MeshFieldPositionZ = 0.0f;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX; // sizeof(g_CubeVertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	/*float x = 0.0f;
	float y = 0.0f;\
	for (int j = 0; j < MESH_V_VERTEX_COUNT; j++) {
		for (int i = 0; i < MESH_H_VERTEX_COUNT; i += 4) {
			g_MeshFieldVertex[i] = { { x - 0.5f, 0.5f,  y + 0.5f}, color, {0.0f,0.0f} };
			g_MeshFieldVertex[i + 1] = { { x + 0.5f, 0.5f, y - 0.5f}, color , {0.25f,0.25f} };
			g_MeshFieldVertex[i + 2] = { { x - 0.5f, 0.5f, y - 0.5f}, color , {0.0f,0.25f} };
			g_MeshFieldVertex[i + 3] = { { x + 0.5f, 0.5f, y + 0.5f}, color , {0.25f,0.0f} };
			x += 1.0f;
		}
		y += 1.0f;
	}*/

	for (int z = 0; z < MESH_V_VERTEX_COUNT; z++) {
		for (int x = 0; x < MESH_H_VERTEX_COUNT; x++) {
			//横　＋　横の最大数　＊　縦
			int index = x + MESH_H_VERTEX_COUNT * z ;
			g_MeshFieldVertex[index].position = {x * MESH_SIZE, 0.0f, z * MESH_SIZE};
			g_MeshFieldVertex[index].normal = {0.0f, 1.0f, 0.0f};
			g_MeshFieldVertex[index].color = {0.0f, 1.0f, 0.0f, 1.0f};
			g_MeshFieldVertex[index].texcoord = {x * 1.0f, z * 1.0f};
		}
	}

	for (int z = 0; z < MESH_V_VERTEX_COUNT; z++) {
		int index = 24 + MESH_H_VERTEX_COUNT * z;
		g_MeshFieldVertex[index].color = { 1.0f, 0.0f, 0.0f, 1.0f };
		 index = 25 + MESH_H_VERTEX_COUNT * z;
		g_MeshFieldVertex[index].color = { 1.0f, 0.0f, 0.0f, 1.0f };
		 index = 26 + MESH_H_VERTEX_COUNT * z;
		g_MeshFieldVertex[index].color = { 1.0f, 0.0f, 0.0f, 1.0f };

	}

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_MeshFieldVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	//インデックスバッファ作成
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX; // sizeof(g_CubeIndex);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	/*int cnt = 0;
	for (int i = 0;i < NUM_INDEX; i+=6) {
		g_MeshFieldIndex[i] = cnt;
		g_MeshFieldIndex[i + 1] = cnt + 1;
		g_MeshFieldIndex[i + 2] = cnt + 2;
		g_MeshFieldIndex[i + 3] = cnt;
		g_MeshFieldIndex[i + 4] = cnt + 3;
		g_MeshFieldIndex[i + 5] = cnt + 1;
		cnt += 4;
	}*/
	int index = 0;
	for (int v = 0;v < MESH_V_COUNT; v++) {
		for (int h = 0;h < MESH_H_COUNT; h++) {
			/*g_MeshFieldIndex[index + 0] = h + (v + 0) * MESH_H_VERTEX_COUNT;
			g_MeshFieldIndex[index + 1] = g_MeshFieldIndex[index + 0] + 1;
			g_MeshFieldIndex[index + 2] = h + (v + 1) * MESH_H_VERTEX_COUNT;
			g_MeshFieldIndex[index + 3] = g_MeshFieldIndex[index + 2];
			g_MeshFieldIndex[index + 4] = g_MeshFieldIndex[index + 1];
			g_MeshFieldIndex[index + 5] = g_MeshFieldIndex[index + 2] + 1;*/

			g_MeshFieldIndex[index + 0] = static_cast<unsigned short>(h + (v + 0) * MESH_H_VERTEX_COUNT);
			g_MeshFieldIndex[index + 1] = static_cast<unsigned short>(h + (v + 1) * MESH_H_VERTEX_COUNT + 1);
			g_MeshFieldIndex[index + 2] = g_MeshFieldIndex[index + 0] + 1;
			g_MeshFieldIndex[index + 3] = g_MeshFieldIndex[index + 0];
			g_MeshFieldIndex[index + 4] = g_MeshFieldIndex[index + 1] - 1;
			g_MeshFieldIndex[index + 5] = g_MeshFieldIndex[index + 1];

			index += 6;
		}
	}

	sd.pSysMem = g_MeshFieldIndex;
	
	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);

	g_MeshFieldTexId0 = Texture_Load(L"resource/texture/grass.png",TRUE);
	g_MeshFieldTexId1 = Texture_Load(L"resource/texture/road.png", TRUE);
	ShaderField_Initialize(g_pDevice, g_pContext);

	//float y = 0;
	//
	//float mesh_size = 1.0f;



	/*for () {
		for () {

		}
	}*/

	//前 - z
	//{ { -0.5, 0.5, -0.5 }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f,0.0f } },		// 0
	//{ { 0.5,-0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.25f,0.25f} },	// 1
	//{ {-0.5,-0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f,0.25f} },		// 2

	//{{-0.5, 0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f,0.0f}},
	//{ { 0.5, 0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.25f,0.0f} },		// 3
	//{{ 0.5,-0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.25f,0.25f}},
}

void Meshfield_Finalize()
{
	ShaderField_Finalize();
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
}

void Meshfield_Draw()
{
	Texture_SetTexture(g_MeshFieldTexId0,0);
	Texture_SetTexture(g_MeshFieldTexId1,1);

	// シェーダーを描画パイプラインに設定
	ShaderField_Begin();

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//float offset_x = MESH_H_COUNT * MESH_SIZE * 0.5f;
	//float offset_z = MESH_V_COUNT * MESH_SIZE * 0.5f;
	////ShaderField_SetWorldMatrix(XMMatrixTranslation(g_MeshFieldPositionX, 0.0f, g_MeshFieldPositionZ));
	////ShaderField_SetWorldMatrix(XMMatrixTranslation(-offset_x, 0.0f, -offset_y));
	//ShaderField_SetWorldMatrix(
	//	XMMatrixTranslation(
	//		g_MeshFieldPositionX - offset_x, 
	//		0.0f,
	//		g_MeshFieldPositionZ - offset_z
	//	)
	//);

	////ShaderField_SetViewMatrix(XMLoadFloat4x4(&Camera_GetMatrix()));

	////ShaderField_SetProjectionMatrix(XMLoadFloat4x4(&Camera_GetPerspectiveMatrix()));

	//ShaderField_SetColor({1.0f,1.0f,1.0f,1.0f});
	////ShaderField_SetColor({0.1f,0.1f,0.1f,1.0f});
	////Shader3D_SetWorldMatrix(XMMatrixIdentity());
	//// 頂点シェーダーにワールド座標変換行列を設定
	////Shader3D_SetWorldMatrix(mtxWorld);

	//// ポリゴン描画命令発行
	//g_pContext->DrawIndexed(NUM_INDEX, 0,0); // g_pContext->Draw(NUM_VERTEX, 0);
	float offset_x = MESH_H_COUNT * MESH_SIZE * 0.5f;
	float offset_z = MESH_V_COUNT * MESH_SIZE * 0.5f;

	const float TILE = MESH_V_COUNT * MESH_SIZE; // 50

	// pick the "base tile" under the player
	XMFLOAT3 p = Player_GetPosition();
	float baseZ = floorf((p.z + offset_z) / TILE) * TILE;

	// draw 3 tiles: behind, current, ahead
	for (int i = -1; i <= 1; ++i)
	{
		float z = baseZ + i * TILE;

		ShaderField_SetWorldMatrix(
			XMMatrixTranslation(
				-offset_x,
				0.0f,
				z - offset_z
			)
		);
		ShaderField_SetColor({ 1.0f,1.0f,1.0f,1.0f });
		g_pContext->DrawIndexed(NUM_INDEX, 0, 0);
	}
}

float Meshfield_GetPositionX()
{
	return g_MeshFieldPositionX;
}

float Meshfield_GetPositionZ()
{
	return g_MeshFieldPositionZ;
}

void Meshfield_SetPositionX(float x) {
	g_MeshFieldPositionX = x;
}

void Meshfield_SetPositionZ(float z) {
	g_MeshFieldPositionZ = z;
}