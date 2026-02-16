/*==============================================================================

   Direct3Dの初期化関連 [direct3d.h]
														 Author : Youhei Sato
														 Date   : 2025/05/12
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef DIRECT3D_H
#define DIRECT3D_H


#include <Windows.h>
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>

#include <string>
#include <vector>
#include <d2d1.h>

// セーフリリースマクロ
#define SAFE_RELEASE(o) if (o) { (o)->Release(); o = NULL; }

// Text info
struct D3DTextEntry {
	std::wstring text;
	float x, y, w, h;
	D2D1::ColorF color;
	IDWriteTextFormat* format;
};

static const D2D1::ColorF BLACK = { 0.0f,0.0f,0.0f };
static const D2D1::ColorF WHITE = { 1.0f,1.0f,1.0f };
static const D2D1::ColorF RED = { 1.0f,0.0f,0.0f,1.0f };
static const D2D1::ColorF BLUE = { 0.0f,0.0f,1.0f,1.0f };
static const D2D1::ColorF YELLOW = { 1.0f,1.0f,0.0f,1.0f };
static const D2D1::ColorF GREEN = { 0.0f,1.0f,0.0f,1.0f };
static const D2D1::ColorF RESULTTEXT = { 0.3f,0.5f,0.1f };

bool Direct3D_Initialize(HWND hWnd); // Direct3Dの初期化
void Direct3D_Finalize(); // Direct3Dの終了処理

void Direct3D_Clear(); // バックバッファのクリア
void Direct3D_Present(); // バックバッファの表示

//バックバッファの大きさの
unsigned int Direct3D_GetBackBufferWidth();		//幅
unsigned int Direct3D_GetBackBufferHeight();	//高さ

//Direct3Dデバイス
ID3D11Device* Direct3D_GetDevice();

//Direct3Dデバイスコンテキスト
ID3D11DeviceContext* Direct3D_GetContext();

//
ID3D11RenderTargetView* Direct3D_GetBackBufferRTV();
void Direct3D_SetBackBuffer();
ID3D11DepthStencilView* Direct3D_GetDepthStencilView();

//αブレンド設定関数
void Direct3D_SetAlphaBlendTranparent(); //透過処理
void Direct3D_SetAlphaBlandAdd();		//加算合成
void Direct3D_SetAlphaBlandNone();		//no blending

//深度バッファの設定
void Direct3D_SetDepthEnable(bool enable);
void Direct3D_SetDepthDepthWriteDisable();
void Direct3D_SetRasterizer(D3D11_FILL_MODE fillMode);

void Direct3D_DrawText(const wchar_t* text, float x, float y, float w, float h,
	float fontSize, D2D1::ColorF color = D2D1::ColorF(D2D1::ColorF::White));

// Text buffer functions
void Direct3D_AddText(const std::wstring & text, float x, float y, float w, float h,
	float fontSize, D2D1::ColorF color = D2D1::ColorF(D2D1::ColorF::White));
void Direct3D_DrawAllText();
void Direct3D_ClearAllText();


#endif // DIRECT3D_H
