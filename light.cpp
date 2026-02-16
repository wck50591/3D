// ========================================================================
//    Name : Light (light.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/30
// ========================================================================

#include "light.h"
#include "direct3d.h"
using namespace DirectX;

static ID3D11Buffer* g_pPSConstantBuffer1 = nullptr;	//定数バッファb1 ambient
static ID3D11Buffer* g_pPSConstantBuffer2 = nullptr;	//定数バッファb2 directional
static ID3D11Buffer* g_pPSConstantBuffer3 = nullptr;	//定数バッファb3 specular
static ID3D11Buffer* g_pPSConstantBuffer4 = nullptr;	//定数バッファb4 point light

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct DirectionalLight {
	XMFLOAT4 directional;
	XMFLOAT4 color;
};

struct SpecularLight
{
	XMFLOAT3 CameraPosition;
	float power;
	XMFLOAT4 Color;
	
};

struct PointLight {
	XMFLOAT3 LightPosition;
	float Range;
	XMFLOAT4 Color;
	//float specularPower;
	//XMFLOAT3 specularColor;
};

struct PointLightList {
	PointLight light[4];
	int count;
	XMFLOAT3 dummy;
};

static PointLightList g_PointLights;

void light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// デバイスとデバイスコンテキストの保存
	g_pDevice = pDevice;
	g_pContext = pContext;

	// 頂点シェーダー用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer1);

	buffer_desc.ByteWidth = sizeof(DirectionalLight); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer2);

	buffer_desc.ByteWidth = sizeof(SpecularLight); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer3);
	
	buffer_desc.ByteWidth = sizeof(PointLightList); // バッファのサイズ
	g_pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer4);

	
}

void light_Finalize()
{
	SAFE_RELEASE(g_pPSConstantBuffer4);
	SAFE_RELEASE(g_pPSConstantBuffer3);
	SAFE_RELEASE(g_pPSConstantBuffer2);
	SAFE_RELEASE(g_pPSConstantBuffer1);
}

void light_SetAmbient(const DirectX::XMFLOAT3& color)
{
	// 定数バッファに行列をセット
	g_pContext->UpdateSubresource(g_pPSConstantBuffer1, 0, nullptr, &color, 0, 0);
	g_pContext->PSSetConstantBuffers(1, 1, &g_pPSConstantBuffer1);
}

void light_SetDirectionalWorld(const DirectX::XMFLOAT4& world_directional, const DirectX::XMFLOAT4& color)
{

	DirectionalLight light{
		world_directional,
		color
	};

	g_pContext->UpdateSubresource(g_pPSConstantBuffer2, 0, nullptr, &light, 0, 0);
	g_pContext->PSSetConstantBuffers(2, 1, &g_pPSConstantBuffer2);
}

void Light_SetSpecularWorld(const DirectX::XMFLOAT3& camera_position, float power, const DirectX::XMFLOAT4& color)
{
	SpecularLight light{
		camera_position,
		power,
		color
	};
	g_pContext->UpdateSubresource(g_pPSConstantBuffer3, 0, nullptr, &light, 0, 0);
	g_pContext->PSSetConstantBuffers(3, 1, &g_pPSConstantBuffer3);
}


void Light_SetPointLightCount(int count)
{
	g_PointLights.count = count;

	g_pContext->UpdateSubresource(g_pPSConstantBuffer4, 0, nullptr, &g_PointLights, 0, 0);
	g_pContext->PSSetConstantBuffers(4, 1, &g_pPSConstantBuffer4);
}

void Light_SetPointLight(int num, const DirectX::XMFLOAT3& light_position, float range, const DirectX::XMFLOAT4& color)
{
	g_PointLights.light[num].LightPosition = light_position;
	g_PointLights.light[num].Range = range;
	//g_PointLights.light[num].Color = color;
	g_PointLights.light[num].Color = {color.x,color.y,color.z, 1.0f};
	
	/*PointLightList list{
		{
			{ {0.0f, 2.0f, 0.0f}, 4.0f, {1.0f,1.0f,1.0f,1.0f} },
			{ {3.0f, 2.0f, 0.0f}, 4.0f, {1.0f,0.0f,0.0f,1.0f} },
			{ {0.0f, 2.0f, 3.0f}, 4.0f, {0.0f,1.0f,0.0f,1.0f} },
			{ {3.0f, 2.0f, 3.0f}, 4.0f, {0.0f,0.0f,1.0f,1.0f} }
		},
		4
	};*/
	/*PointLight light{
		light_position,
		range,
		color
	};*/
	g_pContext->UpdateSubresource(g_pPSConstantBuffer4, 0, nullptr, &g_PointLights, 0, 0);
	g_pContext->PSSetConstantBuffers(4, 1, &g_pPSConstantBuffer4);
}

//void light_SetDirectional(const DirectX::XMFLOAT4& d) {
//	light.directional = d;
//}
//
//XMFLOAT4 light_GetDirectional() {
//	return light.directional;
//}
//
//void light_SetColor(const DirectX::XMFLOAT4& c) {
//	light.color = c;
//}
//
//XMFLOAT4 light_GetColor() {
//	return light.color;
//}
