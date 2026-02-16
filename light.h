
// ========================================================================
//    Name : Light (light.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/30
// ========================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include <d3d11.h>
#include <DirectXMath.h>

void light_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void light_Finalize();
void light_SetAmbient(const DirectX::XMFLOAT3& color);
void light_SetDirectionalWorld(const DirectX::XMFLOAT4& world_directional, const DirectX::XMFLOAT4& color);
void Light_SetSpecularWorld(const DirectX::XMFLOAT3& camera_position ,float power , const DirectX::XMFLOAT4& color);
//void Light_SetPointLightList(const PointLight& point, float range, const DirectX::XMFLOAT4& color);
void Light_SetPointLightCount(int count);
void Light_SetPointLight(int num, const DirectX::XMFLOAT3& light_position, float range, const DirectX::XMFLOAT4& color);
//void light_SetDirectional(const DirectX::XMFLOAT4& d);
//DirectX::XMFLOAT4 light_GetDirectional();
//void light_SetColor(const DirectX::XMFLOAT4& c);
//DirectX::XMFLOAT4 light_GetColor();

#endif // !LIGHT_H
