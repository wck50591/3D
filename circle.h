#ifndef CIRCLE_H
#define CIRCLE_H

#include <d3d11.h>
#include <DirectXMath.h>

void Circle_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Circle_Finalize(void);
void Circle_Draw();

void BuildSphere(float radius = 0.5f, int sliceCount = 20, int stackCount = 20);

#endif // !CIRCLE=H
