
// ========================================================================
//    Name : cube (cube.h)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/09
// ========================================================================

#ifndef CUBE_H
#define CUBE_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "collision.h"

void Cube_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Cube_Finalize(void);
void Cube_Draw(int texId, const DirectX::XMMATRIX& mtxWorld);
AABB Cube_GetAABB(const DirectX::XMFLOAT3& position);

#endif // !CUBE_H
