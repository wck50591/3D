
// ========================================================================
//    Name : Shader for drawing sprite with effect (shader_sprite_effect.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/02/18
// ========================================================================
#include "shader_sprite_effect.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "debug_ostream.h"
#include <fstream>
#include "direct3d.h"
#include "sampler.h"

static ShaderSpriteEffect g_ShaderSpriteEffect;

bool ShaderSpriteEffect::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	HRESULT hr; // 戻り値格納用

	// デバイスとデバイスコンテキストのチェック
	if (!pDevice || !pContext) {
		hal::dout << "Shader_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
		return false;
	}

	// デバイスとデバイスコンテキストの保存
	m_pDevice = pDevice;
	m_pContext = pContext;


	// 事前コンパイル済み頂点シェーダーの読み込み
	std::ifstream ifs_vs("resource/shader/shader_vertex_2d_effect.cso", std::ios::binary);

	if (!ifs_vs) {
		MessageBox(nullptr, "頂点シェーダーの読み込みに失敗しました\n\nshader_vertex_2d_effect.cso", "エラー", MB_OK);
		return false;
	}

	// ファイルサイズを取得
	ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
	std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
	ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

	// バイナリデータを格納するためのバッファを確保
	unsigned char* vsbinary_pointer = new unsigned char[filesize];

	ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
	ifs_vs.close(); // ファイルを閉じる

	// 頂点シェーダーの作成
	hr = m_pDevice->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &m_pVertexShader);

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : 頂点シェーダーの作成に失敗しました" << std::endl;
		delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
		return false;
	}

	// 頂点レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得

	// 頂点レイアウトの作成
	hr = m_pDevice->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &m_pInputLayout);

	delete[] vsbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : 頂点レイアウトの作成に失敗しました" << std::endl;
		return false;
	}


	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(XMFLOAT4X4); // バッファのサイズ
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	m_pDevice->CreateBuffer(&buffer_desc, nullptr, &m_pVSConstantBuffer0);
	m_pDevice->CreateBuffer(&buffer_desc, nullptr, &m_pVSConstantBuffer1);

	// PS Buffer
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.ByteWidth = sizeof(SpriteFX); // バッファのサイズ

	Direct3D_GetDevice()->CreateBuffer(&buffer_desc, nullptr, &m_pPSConstantBuffer0);

	// 事前コンパイル済みピクセルシェーダーの読み込み
	std::ifstream ifs_ps("resource/shader/shader_pixel_2d_effect.cso", std::ios::binary);
	if (!ifs_ps) {
		MessageBox(nullptr, "ピクセルシェーダーの読み込みに失敗しました\n\nshader_pixel_2d_effect.cso", "エラー", MB_OK);
		return false;
	}

	ifs_ps.seekg(0, std::ios::end);
	filesize = ifs_ps.tellg();
	ifs_ps.seekg(0, std::ios::beg);

	unsigned char* psbinary_pointer = new unsigned char[filesize];
	ifs_ps.read((char*)psbinary_pointer, filesize);
	ifs_ps.close();

	// ピクセルシェーダーの作成
	hr = m_pDevice->CreatePixelShader(psbinary_pointer, filesize, nullptr, &m_pPixelShader);

	delete[] psbinary_pointer; // バイナリデータのバッファを解放

	if (FAILED(hr)) {
		hal::dout << "Shader_Initialize() : ピクセルシェーダーの作成に失敗しました" << std::endl;
		return false;
	}

	return true;
}

void ShaderSpriteEffect::Finalize()
{
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pPSConstantBuffer0);
	SAFE_RELEASE(m_pVSConstantBuffer1);
	SAFE_RELEASE(m_pVSConstantBuffer0);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
}

void ShaderSpriteEffect::SetProjectionMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	m_pContext->UpdateSubresource(m_pVSConstantBuffer0, 0, nullptr, &transpose, 0, 0);
}

void ShaderSpriteEffect::SetWorldMatrix(const DirectX::XMMATRIX& matrix)
{
	// 定数バッファ格納用行列の構造体を定義
	XMFLOAT4X4 transpose;

	// 行列を転置して定数バッファ格納用行列に変換
	XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

	// 定数バッファに行列をセット
	m_pContext->UpdateSubresource(m_pVSConstantBuffer1, 0, nullptr, &transpose, 0, 0);
}

void ShaderSpriteEffect::SetGlitch(float time, float intensity, float sliceHeight)
{
	SpriteFX glitch{};
	glitch.time = time;
	glitch.intensity = intensity;
	glitch.sliceHeight = sliceHeight;

	m_pContext->UpdateSubresource(m_pPSConstantBuffer0,0,nullptr,&glitch,0,0);
}

void ShaderSpriteEffect::Begin()
{
	// 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
	m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// 頂点レイアウトを描画パイプラインに設定
	m_pContext->IASetInputLayout(m_pInputLayout);

	// 定数バッファを描画パイプラインに設定
	m_pContext->VSSetConstantBuffers(0, 1, &m_pVSConstantBuffer0);
	m_pContext->VSSetConstantBuffers(1, 1, &m_pVSConstantBuffer1);

	m_pContext->PSSetConstantBuffers(0, 1, &m_pPSConstantBuffer0);
}
