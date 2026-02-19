
// ========================================================================
//    Name : Shader for Post Effect (shader_post.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/29
// ========================================================================

#ifndef SHADER_POST_H
#define SHADER_POST_H

#include <d3d11.h>
#include <DirectXMath.h>

bool ShaderPost_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void ShaderPost_Finalize();

struct PostCBuffer
{
    DirectX::XMFLOAT4 color { 1.0f, 1.0f, 1.0f, 1.0f };
    float ivColor { 0.0f };
    float gamma { 1.0f };
    DirectX::XMFLOAT2 dum {};
};

void ShaderPost_SetColor(const DirectX::XMFLOAT4& color);
void ShaderPost_SetInverseColor(float ivColor);
void ShaderPost_SetGamma(float gamma);

void ShaderPost_Begin();

#endif // !SHADER_POST_H