
// ========================================================================
//    Name : sky (sky.cpp)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/21
// ========================================================================

#include "sky.h"
using namespace DirectX;
#include "model.h"
#include "shader3d_unlit.h"

static MODEL* g_pModelSky = nullptr;
static XMFLOAT3 g_Position = {};

void Sky_Initialize()
{
    g_pModelSky = ModelLoad("resource/model/sky2.fbx", 25.0f, true);

}

void Sky_Finalize()
{
    ModelRelease(g_pModelSky);
}

void Sky_SetPosition(const DirectX::XMFLOAT3& position)
{
    g_Position = position;
}

void Sky_Draw()
{
    Shader3DUnlit_Begin();

    g_Position.y = 0;
    XMMATRIX r = XMMatrixRotationX(XMConvertToRadians(90.0f));
    r *= XMMatrixRotationY(-XMConvertToRadians(90.0f));
    ModelUnlitDraw(g_pModelSky, r * XMMatrixTranslationFromVector(XMLoadFloat3(&g_Position)));
}
