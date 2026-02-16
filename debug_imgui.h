#ifndef DEBUG_IMGUI_H
#define DEBUG_IMGUI_H

#include <d3d11.h>

void Imgui_Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Imgui_Finalize();
void Imgui_Update();
void Imgui_Show();
void Imgui_Toggle();
void Imgui_Clear();

#endif // !DEBUG_IMGUI_H
