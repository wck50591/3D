// collision.cpp
#include "collision.h"
#include "direct3d.h"
#include "Texture.h"
#include "shader.h"
#include "shader3d.h"
#include "cube.h"
using namespace DirectX;
#include <algorithm>

static constexpr int NUM_VERTEX = 5000; // 頂点数
static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static int g_WhiteTexId = -1;

bool Collision_IsOverlapCircle(const Circle& a, const Circle& b)
{
    //float xl = b.center.x - a.center.x;
    //float yl = b.center.y - a.center.y;

    //return (a.radius + b.radius) * (a.radius + b.radius) > (xl * xl + yl * yl);
    
    XMVECTOR ac = XMLoadFloat2(&a.center);
    XMVECTOR bc = XMLoadFloat2(&b.center);
    XMVECTOR lsq = XMVector2LengthSq(bc - ac);

    return (a.radius + b.radius) * (a.radius + b.radius) > XMVectorGetX(lsq);
}

bool Collision_IsOverlapBox(const Box& a, const Box& b)
{
    float at = a.center.y - a.half_height;
    float ab = a.center.y + a.half_height;
    float al = a.center.x - a.half_width;
    float ar = a.center.x + a.half_width;

    float bt = b.center.y - b.half_height;
    float bb = b.center.y + b.half_height;
    float bl = b.center.x - b.half_width;
    float br = b.center.x + b.half_width;

    return al < br && ar > bl && at < bb && ab > bt;
}

bool Collision_IsOverlapAABB(const AABB& a, const AABB& b)
{
	return a.min.x < b.max.x
		&& a.max.x > b.min.x
		&& a.min.y < b.max.y
		&& a.max.y > b.min.y
		&& a.min.z < b.max.z
		&& a.max.z > b.min.z;
}

Hit Collision_IsHitAABB(const AABB& a, const AABB& b)
{
	Hit hit{};

	hit.isHit = Collision_IsOverlapAABB(a, b);

	if (!hit.isHit) {
		return hit;
	}
	
	float xdepth = std::min(a.max.x, b.max.x) - std::max(a.min.x,b.min.x);
	float ydepth = std::min(a.max.y, b.max.y) - std::max(a.min.y,b.min.y);
	float zdepth = std::min(a.max.z, b.max.z) - std::max(a.min.z,b.min.z);

	float isShallowX = false;
	float isShallowY = false;
	float isShallowZ = false;

	//一番深度浅い面は？
	if (xdepth > ydepth) {
		if (ydepth > zdepth) {
			// from z
			isShallowZ = true;
		}
		else {
			// from y
			isShallowY = true;
		}
	}
	else {
		if (zdepth > xdepth) {
			// from x
			isShallowX = true;
		}
		else {
			// from z
			isShallowZ = true;
		}
	}

	XMFLOAT3 a_center = a.GetCenter();
	XMFLOAT3 b_center = b.GetCenter();
	XMVECTOR normal = XMLoadFloat3(&b_center) - XMLoadFloat3(&a_center);

	if (isShallowX) {
		normal = XMVector3Normalize(normal * XMVECTOR{ 1.0f, 0.0f, 0.0f});
	}
	else if (isShallowY) {
		normal = XMVector3Normalize(normal * XMVECTOR{ 0.0f, 1.0f, 0.0f });

	}else{
		normal = XMVector3Normalize(normal * XMVECTOR{ 0.0f, 0.0f, 1.0f });
	}

	XMStoreFloat3(&hit.normal, normal);
	return hit;
}

// 頂点構造体
struct Vertex
{
    XMFLOAT3 position; // 頂点座標
    XMFLOAT4 color;		//color
    XMFLOAT2 uv;	//uv/テクスチャ座標
};

void Collision_DebugInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

    g_WhiteTexId = Texture_Load(L"resource/texture/white.png", TRUE);
}

void Collision_DebugFinalize()
{
    SAFE_RELEASE(g_pVertexBuffer);
}

void Collision_DebugDraw(const Circle& circle,const DirectX::XMFLOAT4& color)
{
    // 点の数を算出
    int numVertex = (int)(circle.radius * 2.0f * XM_PI + 1);

	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	const float rad = XM_2PI / numVertex;

	for (int i = 0; i < numVertex; i++) {
		v[i].position.x = cosf(rad * i) * circle.radius + circle.center.x;
		v[i].position.y = sinf(rad * i) * circle.radius + circle.center.y;
		v[i].position.z = 0.0f;
		v[i].color = color;
		v[i].uv = { 0.0f, 0.0f };
	}

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//ワールド変換行列を設定
	Shader_SetWorldMatrix(XMMatrixIdentity());

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//テクスチャの設定
	Texture_SetTexture(g_WhiteTexId);

	// ポリゴン描画命令発行
	g_pContext->Draw(numVertex, 0);
}

void Collision_DebugDraw(const Box& box, const DirectX::XMFLOAT4& color)
{
	// シェーダーを描画パイプラインに設定
	Shader_Begin();

	// 頂点バッファをロックする
	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	// 頂点バッファへの仮想ポインタを取得
	Vertex* v = (Vertex*)msr.pData;

	// 頂点情報を書き込み
	v[0].position = { box.center.x - box.half_width, box.center.y - box.half_height, 0.0f };
	v[1].position = { box.center.x + box.half_width, box.center.y - box.half_height, 0.0f };
	v[2].position = { box.center.x + box.half_width, box.center.y + box.half_height, 0.0f };
	v[3].position = { box.center.x - box.half_width, box.center.y + box.half_height, 0.0f };
	v[4].position = { box.center.x - box.half_width, box.center.y - box.half_height, 0.0f };

	for (int i = 0; i < 5; i++) {
		v[i].color = color;
		v[i].uv = { 0.0f, 0.0f };
	}

	// 頂点バッファのロックを解除
	g_pContext->Unmap(g_pVertexBuffer, 0);

	//ワールド変換行列を設定
	Shader_SetWorldMatrix(XMMatrixIdentity());

	// 頂点バッファを描画パイプラインに設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//テクスチャの設定
	Texture_SetTexture(g_WhiteTexId);

	// ポリゴン描画命令発行
	g_pContext->Draw(5, 0);
}

void Collision_DebugDraw(const AABB& aabb, const DirectX::XMFLOAT4& color)
{
	Shader3D_Begin();

	D3D11_MAPPED_SUBRESOURCE msr;
	g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	Vertex* v = (Vertex*)msr.pData;

	// 8 corners
	XMFLOAT3 p000{ aabb.min.x, aabb.min.y, aabb.min.z };
	XMFLOAT3 p001{ aabb.min.x, aabb.min.y, aabb.max.z };
	XMFLOAT3 p010{ aabb.min.x, aabb.max.y, aabb.min.z };
	XMFLOAT3 p011{ aabb.min.x, aabb.max.y, aabb.max.z };
	XMFLOAT3 p100{ aabb.max.x, aabb.min.y, aabb.min.z };
	XMFLOAT3 p101{ aabb.max.x, aabb.min.y, aabb.max.z };
	XMFLOAT3 p110{ aabb.max.x, aabb.max.y, aabb.min.z };
	XMFLOAT3 p111{ aabb.max.x, aabb.max.y, aabb.max.z };

	auto setLine = [&](int idx, const XMFLOAT3& a, const XMFLOAT3& b)
		{
			v[idx + 0].position = a;
			v[idx + 1].position = b;
			v[idx + 0].color = color;
			v[idx + 1].color = color;
			v[idx + 0].uv = { 0,0 };
			v[idx + 1].uv = { 0,0 };
		};

	// 12 edges => 24 vertices
	int k = 0;

	// bottom rectangle
	setLine(k, p000, p001); k += 2;
	setLine(k, p001, p101); k += 2;
	setLine(k, p101, p100); k += 2;
	setLine(k, p100, p000); k += 2;

	// top rectangle
	setLine(k, p010, p011); k += 2;
	setLine(k, p011, p111); k += 2;
	setLine(k, p111, p110); k += 2;
	setLine(k, p110, p010); k += 2;

	// vertical edges
	setLine(k, p000, p010); k += 2;
	setLine(k, p001, p011); k += 2;
	setLine(k, p101, p111); k += 2;
	setLine(k, p100, p110); k += 2;

	g_pContext->Unmap(g_pVertexBuffer, 0);

	Shader3D_SetWorldMatrix(XMMatrixIdentity());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Texture_SetTexture(g_WhiteTexId);

	g_pContext->Draw(24, 0);
}

