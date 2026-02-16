
// ========================================================================
//    Name : material(material.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/13
// ========================================================================

#include "material.h"
#include <d3d11.h>
#include "direct3d.h"

using namespace DirectX;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

static ID3D11Buffer* g_pEmissiveCB = nullptr; // PS b5

struct EmissiveCB
{
    XMFLOAT4 emissive;
};

void Material_Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
    g_pDevice = device;
    g_pContext = context;

    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(EmissiveCB);

    g_pDevice->CreateBuffer(&bd, nullptr, &g_pEmissiveCB);
}

void Material_Finalize()
{
    SAFE_RELEASE(g_pEmissiveCB);
}

void Material_SetEmissive(const XMFLOAT4& emissive)
{
    EmissiveCB cb{ emissive };
    g_pContext->UpdateSubresource(g_pEmissiveCB, 0, nullptr, &cb, 0, 0);
    g_pContext->PSSetConstantBuffers(5, 1, &g_pEmissiveCB);
}
