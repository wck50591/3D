
// ========================================================================
//    Name : ビルボード(billboard.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/14
// ========================================================================

#include "billboard.h"
#include "shader_billboard.h"
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "texture.h"
#include "player_camera.h"

static constexpr int NUM_VERTEX = 4; // 頂点数

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

static XMFLOAT4X4 g_mtxView = {}; 

struct VertexBillboard
{
	XMFLOAT3 position;  // 頂点座標
	XMFLOAT4 color;		// color
	XMFLOAT2 texcoord;  // UV
};

void Billboard_Initialize()
{
	ShaderBillboard_Initialize();

	static VertexBillboard g_Vertex[NUM_VERTEX] = {

		{{-0.5f, 0.5f, 0.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f , 0.0f}},
		{{ 0.5f, 0.5f, 0.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f , 0.0f}},
		{{-0.5f,-0.5f, 0.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f , 1.0f}},
		{{ 0.5f,-0.5f, 0.0f}, { 1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f , 1.0f}},
	};


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexBillboard) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = g_Vertex;

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pVertexBuffer);

}

void Billboard_Finalize()
{
	SAFE_RELEASE(g_pVertexBuffer);
}

void Billboard_SetViewMatrix(const DirectX::XMFLOAT4X4& view)
{
	//カメラ行列の平行移動成分をカット
	g_mtxView = view;
	g_mtxView._41 = g_mtxView._42 = g_mtxView._43 = 0.0f;

}

void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale
	, const DirectX::XMFLOAT4& color, float contrast, const XMFLOAT2& pivot)
{

	// シェーダーを描画パイプラインに設定
	ShaderBillboard_Begin();

	ShaderBillboard_SetUVParameter({ {1.0f, 1.0f}, {0.0f, 0.0f} });

	//ピクセルシェーダーに色を設定
	ShaderBillboard_SetColor({ 1.0f,1.0f,1.0f,1.0f });

	Texture_SetTexture(texId);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(VertexBillboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	//Direct3D_GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	// 頂点シェーダーにワールド座標変換行列を設定

	// 直交行列の逆行列は転置行列に
	XMMATRIX iv = XMMatrixTranspose(XMLoadFloat4x4(&g_mtxView));

	// 回転軸までのオフセット行列
	XMMATRIX pivot_offset = XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);

	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, 1.0f);
	XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);
	ShaderBillboard_SetWorldMatrix(s * pivot_offset * iv * t);

	// ポリゴン描画命令発行
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);

}

void Billboard_Draw(int texId, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& scale, const DirectX::XMUINT4& tex_cut, const DirectX::XMFLOAT2& pivot)
{

	// シェーダーを描画パイプラインに設定
	ShaderBillboard_Begin();

	float uv_x = (float)tex_cut.x / Texture_Width(texId);
	float uv_y = (float)tex_cut.y / Texture_Height(texId);
	float uv_w = (float)tex_cut.z / Texture_Width(texId);
	float uv_h = (float)tex_cut.w / Texture_Height(texId);

	ShaderBillboard_SetUVParameter({ {uv_w, uv_h}, {uv_x, uv_y} });

	//ピクセルシェーダーに色を設定
	ShaderBillboard_SetColor({ 1.0f,1.0f,1.0f,1.0f });

	Texture_SetTexture(texId);

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(VertexBillboard);
	UINT offset = 0;
	Direct3D_GetContext()->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// インデックスバッファを描画パイプラインに設定
	//Direct3D_GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 頂点シェーダーにワールド座標変換行列を設定

	//カメラ行列の回転だけ逆行列を作る
	//XMMATRIX iv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&g_mtxView));

	// 直交行列の逆行列は転置行列に
	XMMATRIX iv = XMMatrixTranspose(XMLoadFloat4x4(&g_mtxView));

	// 回転軸までのオフセット行列
	XMMATRIX pivot_offset = XMMatrixTranslation(-pivot.x, -pivot.y, 0.0f);

	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, 1.0f);
	XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);
	ShaderBillboard_SetWorldMatrix(s * pivot_offset * iv * t);

	// ポリゴン描画命令発行
	Direct3D_GetContext()->Draw(NUM_VERTEX, 0);
}
