
// ========================================================================
//    Name : Post Effect (post_effect.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/29
// ========================================================================

#include "post_effect.h"
#include "direct3d.h"
#include "shader_post.h"

static ID3D11Texture2D* g_pSceneTex = nullptr;
static ID3D11RenderTargetView* g_pSceneRTV = nullptr;
static ID3D11ShaderResourceView* g_pSceneSRV = nullptr;  // テクスチャ
static ID3D11ShaderResourceView* g_pNullSRV = nullptr;

// from directx3d, no need to release
static ID3D11RenderTargetView* g_pMainRTV = nullptr;

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11Buffer* g_pPostScreenVB = nullptr;

struct PostScreen {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

static PostScreen ps[6] = {

	// left
	{ {-1.0f, -1.0f, 0.0f }, { 0.0, 1.0 } },
	{ {-1.0f,  1.0f, 0.0f }, { 0.0, 0.0 } },
	{ { 1.0f,  1.0f, 0.0f }, { 1.0, 0.0 } },

	// right
	{ {-1.0f, -1.0f, 0.0f }, { 0.0, 1.0 } },
	{ { 1.0f,  1.0f, 0.0f }, { 1.0, 0.0 } },
	{ { 1.0f, -1.0f, 0.0f }, { 1.0, 1.0 } }

};



bool PostEffect_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr;

	g_pDevice = pDevice;
	g_pContext = pContext;

	D3D11_TEXTURE2D_DESC td{};
	td.Width = Direct3D_GetBackBufferWidth();
	td.Height = Direct3D_GetBackBufferHeight();
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = g_pDevice->CreateTexture2D(&td, nullptr, &g_pSceneTex);
	if (FAILED(hr)) return false;

	hr = g_pDevice->CreateRenderTargetView(g_pSceneTex, nullptr, &g_pSceneRTV);
	if (FAILED(hr)) return false;

	hr = g_pDevice->CreateShaderResourceView(g_pSceneTex, nullptr, &g_pSceneSRV);
	if (FAILED(hr)) return false;

	//Post Effect Screen Initialize

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(PostScreen) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = ps;

	Direct3D_GetDevice()->CreateBuffer(&bd, &sd, &g_pPostScreenVB);

	return true;
}

void PostEffect_Finalize()
{
	SAFE_RELEASE(g_pPostScreenVB);
	SAFE_RELEASE(g_pNullSRV);
	SAFE_RELEASE(g_pSceneSRV);
	SAFE_RELEASE(g_pSceneRTV);
	SAFE_RELEASE(g_pSceneTex);
}

void PostEffect_Clear()
{
	float clear_color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
	g_pContext->ClearRenderTargetView(g_pSceneRTV, clear_color);

	g_pContext->ClearDepthStencilView(Direct3D_GetDepthStencilView(),D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	PostEffect_SetBackBuffer();
}

void PostEffect_Begin()
{
	g_pMainRTV = Direct3D_GetBackBufferRTV();

	// send texture
	g_pContext->OMSetRenderTargets(1, &g_pMainRTV, nullptr);

	g_pContext->PSSetShaderResources(0, 1, &g_pSceneSRV);

	// set for draw content to post effect screen
	Direct3D_SetDepthEnable(false);
	Direct3D_SetAlphaBlandNone();

	ShaderPost_Begin();
}

void PostEffect_End()
{
	g_pContext->PSSetShaderResources(0, 1, &g_pNullSRV);

	// reset to normal
	Direct3D_SetBackBuffer();
	Direct3D_SetDepthEnable(true);
	Direct3D_SetAlphaBlendTranparent();
}

void PostEffect_Draw()
{
	PostScreen_Draw();
}

void PostEffect_SetBackBuffer()
{
	g_pContext->OMSetRenderTargets(1, &g_pSceneRTV, Direct3D_GetDepthStencilView());
}

void PostEffect_SetInverseColor(float strength)
{
	ShaderPost_SetInverseColor(strength);
}

void PostEffect_SetGamma(float strength)
{
	ShaderPost_SetGamma(strength);
}

void PostScreen_Draw() {
	UINT stride = sizeof(PostScreen);
	UINT offset = 0;
	g_pContext->IASetVertexBuffers(0, 1, &g_pPostScreenVB, &stride, &offset);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pContext->Draw(6, 0);
}