
// ========================================================================
//    Name : 3Dシェーダー (shader3d.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/10
// ========================================================================

#ifndef SHADER3D_H
#define	SHADER3D_H

#include <d3d11.h>
#include <DirectXMath.h>

bool Shader3D_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader3D_Finalize();

void Shader3D_SetWorldMatrix(const DirectX::XMMATRIX& matrix);

void Shader3D_SetColor(const DirectX::XMFLOAT4& color);
void Shader3D_SetEmissive(const DirectX::XMFLOAT4& e);
void Shader3D_Begin();

#endif // SHADER3D_H
