
// ========================================================================
//    Name : cube (cube.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/09
// ========================================================================

#include "cube.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "shader3d.h"
#include "texture.h"


static constexpr int NUM_VERTEX = 4 * 6; // 頂点数
static constexpr int NUM_INDEX = 3 * 2 * 6; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
static ID3D11Buffer* g_pIndexBuffer = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

//static int g_CubeTexId = -1;

// 3D頂点構造体
struct Vertex3D
{
	XMFLOAT3 position;  // 頂点座標
	XMFLOAT3 normal;	// 法線
	XMFLOAT4 color;		// color
	XMFLOAT2 texcoord;  // UV
};

static Vertex3D g_CubeVertex[NUM_VERTEX] = {

	//前 - z
	{{-0.5, 0.5,-0.5}, {0.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,0.0f}},		// 0
	{{ 0.5,-0.5,-0.5}, {0.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.25f}},	// 1
	{{-0.5,-0.5,-0.5}, {0.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f }, {0.0f,0.25f}},		// 2

	//{{-0.5, 0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f,0.0f}},
	{{ 0.5, 0.5,-0.5}, {0.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.0f}},		// 3
	//{{ 0.5,-0.5,-0.5}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.25f,0.25f}},

	//後 - z 
	{{-0.5, 0.5, 0.5}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f,0.0f}},		// 4
	{{-0.5,-0.5, 0.5}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f,0.25f}},		// 5
	{{ 0.5,-0.5, 0.5}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.25f}},	// 6
	
	//{{-0.5, 0.5, 0.5}, {0.3f, 0.3f, 1.0f, 1.0f}, {0.5f,0.0f}},
	//{{ 0.5,-0.5, 0.5}, {0.3f, 0.3f, 1.0f, 1.0f}, {0.25f,0.25f}},
	{{ 0.5, 0.5, 0.5}, {0.0f,0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.0f}},		// 7
	
	
	//上 - y
	{{-0.5, 0.5, 0.5}, {0.0f,1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f,0.0f}},		// 8
	{{ 0.5, 0.5,-0.5}, {0.0f,1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.75f,0.25f}},	// 9
	{{-0.5, 0.5,-0.5}, {0.0f,1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.5f,0.25f}},		// 10

	//{{-0.5, 0.5, 0.5}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.5f,0.0f}},
	{{ 0.5, 0.5, 0.5}, {0.0f,1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.75f,0.0f}},		// 11
	//{{ 0.5, 0.5,-0.5}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.75f,0.25f}},
	
	//下 - y
	{{ 0.5,-0.5,-0.5}, {0.0f,-1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f,0.0f}},		// 12
	{{-0.5,-0.5, 0.5}, {0.0f,-1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.75f,0.25f}},	// 13
	{{-0.5,-0.5,-0.5}, {0.0f,-1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.75f,0.0f}},		// 14

	//{{ 0.5,-0.5,-0.5}, {0.3f, 1.0f, 0.3f, 1.0f}, {1.0f,0.0f}},
	{{ 0.5,-0.5, 0.5}, {0.0f,-1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f,0.25f}},		// 15
	//{{-0.5,-0.5, 0.5}, {0.3f, 1.0f, 0.3f, 1.0f}, {0.75f,0.25f}},
	
	//左 - x
	{{-0.5, 0.5, 0.5}, {-1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.5f}},		// 16
	{{-0.5,-0.5,-0.5}, {-1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,0.25f}},		// 17
	{{-0.5,-0.5, 0.5}, {-1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.25f}},	// 18

	//{{-0.5, 0.5, 0.5}, {1.0f, 0.3f, 0.3f, 1.0f}, {0.0f,0.0f}},
	{{-0.5, 0.5,-0.5}, {-1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.0f}},		// 19
	//{{-0.5,-0.5,-0.5}, {1.0f, 0.3f, 0.3f, 1.0f}, {0.25f,0.25f}},

	//右 - x
	{{ 0.5, 0.5,-0.5}, {1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,0.0f}},		// 20
	{{ 0.5,-0.5, 0.5}, {1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.25f}},	// 21
	{{ 0.5,-0.5,-0.5}, {1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f,0.25f}},		// 22

	//{{ 0.5, 0.5,-0.5}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f,0.0f}},
	{{ 0.5, 0.5, 0.5}, {1.0f,0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.25f,0.0f}},		// 23
	//{{ 0.5,-0.5, 0.5}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.25f,0.25f}},

};

static unsigned short g_CubeIndex[]{
	0,1,2,0,3,1,
	4,5,6,4,6,7,
	8,9,10,8,11,9,
	12,13,14,12,15,13,
	16,17,18,16,19,17,
	20,21,22,20,23,21
};

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex3D) * NUM_VERTEX; // sizeof(g_CubeVertex);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_CubeVertex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

	//インデックスバッファ作成
	bd.ByteWidth = sizeof(unsigned short) * NUM_INDEX; // sizeof(g_CubeIndex);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	sd.pSysMem = g_CubeIndex;

	g_pDevice->CreateBuffer(&bd, &sd, &g_pIndexBuffer);

	//g_CubeTexId = Texture_Load(L"resource/texture/number_512x512.png",TRUE);
}

void Cube_Finalize(void)
{
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexBuffer);
}

void Cube_Draw(int texid, const DirectX::XMMATRIX& mtxWorld)
{
	// シェーダーを描画パイプラインに設定
	Shader3D_Begin();

	//ピクセルシェーダーに色を設定
	Shader3D_SetColor({1.0f,1.0f,1.0f,1.0f});

	Texture_SetTexture(texid);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	g_pContext->IASetIndexBuffer(g_pIndexBuffer,DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点シェーダーにワールド座標変換行列を設定
	Shader3D_SetWorldMatrix(mtxWorld);

	// ポリゴン描画命令発行
	g_pContext->DrawIndexed(NUM_INDEX, 0, 0); // g_pContext->Draw(NUM_VERTEX, 0);

	// ワールド座標変換行列の作成
	//XMMATRIX mtxWorld = XMMatrixIdentity(); // 単位行列の作成
	//mtxWorld = XMMatrixTranslation(0.5f, 0.5f, 0.0f);
	//float x = -4.5f;
	//float z = 4.5f;
	//for (int i = 0; i < 10; i++) {
	//	/*for (int j = 0; j < 10; j++) {
	//		
	//		z += 1.0f;
	//	}*/
	//	mtxWorld = XMMatrixTranslation(x, 0.5f, z);
	//	Shader3D_SetWorldMatrix(mtxWorld);
	//	g_pContext->Draw(NUM_VERTEX, 0);
	//	x += 1.0f;
	//}

	//XMMATRIX mtxRot = XMMatrixRotationY(g_angle); // 回転行列

	//for (int a = 0; a < 2;a++) {

	//	XMMATRIX mtxTrans = XMMatrixTranslation(-20.0f - a * 20, 0.0f, 0.0f);

	//	int d = 1;
	//	for (int k = 0; k < 10 * d; k++) {
	//		for (int i = 0; i < 10 * d - k; i++) {
	//			for (int j = 0; j < 10 * d - k; j++) {
	//				XMMATRIX mtx = XMMatrixTranslation(-4.5f * d + i + 0.5f * k, 0.5f + k, -4.5f * d + j + 0.5f * k);
	//				XMMATRIX mtxWorld = mtx * mtxRot * mtxTrans;
	//				Shader3D_SetWorldMatrix(mtxWorld);
	//				g_pContext->Draw(NUM_VERTEX, 0);
	//			}
	//		}
	//	}
	//}

	//XMMATRIX mtxTrans = XMMatrixTranslation(-4.5, 0.5f, 0.0f); // 並行移動行列
	//XMMATRIX mtxWorld = XMMatrixTranslation(g_x, 0.5f, 0.0f); // 並行移動行列
	//XMMATRIX mtxWorld = XMMatrixRotationY(g_angle); // 回転行列
	//XMMATRIX mtxWorld = XMMatrixScaling(1.0f, g_scale, 1.0f); // 拡大行列

	//XMMATRIX mtxTrans = XMMatrixTranslation(-4.5, 0.5f, 0.0f); // 並行移動行列
	//XMMATRIX mtxRot = XMMatrixRotationY(g_angle); // 回転行列
	//XMMATRIX mtxWorld = mtxRot * mtxTrans;

	//XMMATRIX mtxTrans = XMMatrixTranslation(-0.0f, 0.5f, 0.0f);
	//XMMATRIX mtxScale = XMMatrixScaling(1.0f, g_scale, 1.0f); // 拡大行列
	//XMMATRIX mtxTrans2 = XMMatrixTranslation(-4.5, 0.5f, 0.0f); // 並行移動行列
	//XMMATRIX mtxWorld = mtxTrans * mtxScale;//*mtxTrans2;

	//XMMATRIX mtxTrans = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	//XMMATRIX mtxRot = XMMatrixRotationY(g_angle); // 回転行列
	//XMMATRIX mtxRot2 = XMMatrixRotationZ(XMConvertToRadians(90.0f)); // 回転行列
	//XMMATRIX mtxScale = XMMatrixScaling(1.0f, 5.0f, 1.0f); // 拡大行列
	//XMMATRIX mtxWorld = mtxTrans * mtxScale * mtxRot2 * mtxRot;

	//XMMATRIX mtxTrans = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	//XMMATRIX mtxScale = XMMatrixScaling(5.0f, 1.0f, 1.0f); // 拡大行列
	//XMMATRIX mtxRot = XMMatrixRotationY(g_angle); // 回転行列
	//XMMATRIX mtxTrans2 = XMMatrixTranslation(2.5f, 0.5f, 0.0f);// 回転行列
	//XMMATRIX mtxWorld = mtxTrans * mtxScale * mtxRot;//* mtxTrans2;

	//XMMATRIX mtxTrans = XMMatrixTranslation(0.5f, 0.0f, 0.0f);
	//XMMATRIX mtxTrans2 = XMMatrixTranslation(-0.5f, 0.0f, 0.0f);
	//XMMATRIX mtxRot = XMMatrixRotationY(g_angle); // 回転行列
	//XMMATRIX mtxScale = XMMatrixScaling(4.0f, 1.0f, 1.0f); // 拡大行列
	//XMMATRIX mtxWorld = mtxTrans * mtxScale * mtxTrans2 * mtxRot;

	
}

AABB Cube_GetAABB(const DirectX::XMFLOAT3& position)
{
	return { 
		{
			position.x - 0.5f, 
			position.y - 0.5f, 
			position.z - 0.5f
		},
		{
			position.x + 0.5f,
			position.y + 0.5f,
			position.z + 0.5f
		} 
	};
}
