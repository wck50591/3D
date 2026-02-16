
// ========================================================================
//    Name : material(material.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/13
// ========================================================================

#ifndef MATERIAL_H
#define MATERIAL_H
#include <d3d11.h>
#include <DirectXMath.h>

void Material_Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
void Material_Finalize();

void Material_SetEmissive(const DirectX::XMFLOAT4& emissive);

#endif // !MATERIAL_H