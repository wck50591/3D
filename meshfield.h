
// ========================================================================
//    Name : mesh field(meshfield.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/19
// ========================================================================

#ifndef MESHFIELD_H
#define MESHFIELD_H

#include <d3d11.h>
#include <DirectXMath.h>

void Meshfield_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Meshfield_Finalize();
void Meshfield_Draw();

float Meshfield_GetPositionX();
float Meshfield_GetPositionZ();
void Meshfield_SetPositionX(float x);
void Meshfield_SetPositionZ(float z);

#endif // !MESHFIELD_H
