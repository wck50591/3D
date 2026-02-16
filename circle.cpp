#include "circle.h"

#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
#include "shader3d.h"
#include <vector>

static constexpr int SEGMENTS = 36;
static constexpr int NUM_VERTEX_CIRCLE = SEGMENTS * 3; // each segment = 1 triangle (3 verts)

static ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ

// 注意！初期化で外部から設定されるもの。Release不要。
static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;

struct Vertex3D
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

static std::vector<Vertex3D> g_SphereVertices;

void Circle_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    g_pDevice = pDevice;
    g_pContext = pContext;

    BuildSphere(1.0f, 50.0f, 50.0f); // radius=0.5, smooth sphere

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex3D) * (UINT)g_SphereVertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = g_SphereVertices.data();

    g_pDevice->CreateBuffer(&bd, &sd, &g_pVertexBuffer);
}

void Circle_Finalize(void)
{
}

void Circle_Draw()
{
    Shader3D_Begin();

    UINT stride = sizeof(Vertex3D);
    UINT offset = 0;
    g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX mtxtrans = XMMatrixTranslation(5.0f,10.0f,0.0f);
    XMMATRIX mtxWorld = mtxtrans;

    Shader3D_SetWorldMatrix(mtxWorld);

    g_pContext->Draw((UINT)g_SphereVertices.size(), 0);
}

void BuildSphere(float radius, int sliceCount, int stackCount)
{
    g_SphereVertices.clear();

    XMFLOAT4 col = { 0.3f, 0.6f, 1.0f, 1.0f }; // light blue
    float dTheta = XM_2PI / sliceCount;
    float dPhi = XM_PI / stackCount;

    for (int i = 0; i < stackCount; i++)
    {
        float phi1 = i * dPhi;
        float phi2 = (i + 1) * dPhi;

        for (int j = 0; j < sliceCount; j++)
        {
            float theta1 = j * dTheta;
            float theta2 = (j + 1) * dTheta;

            // 4 points of the current quad
            XMFLOAT3 p1 = { radius * sinf(phi1) * cosf(theta1), radius * cosf(phi1), radius * sinf(phi1) * sinf(theta1) };
            XMFLOAT3 p2 = { radius * sinf(phi2) * cosf(theta1), radius * cosf(phi2), radius * sinf(phi2) * sinf(theta1) };
            XMFLOAT3 p3 = { radius * sinf(phi2) * cosf(theta2), radius * cosf(phi2), radius * sinf(phi2) * sinf(theta2) };
            XMFLOAT3 p4 = { radius * sinf(phi1) * cosf(theta2), radius * cosf(phi1), radius * sinf(phi1) * sinf(theta2) };

            // Triangle 1 (p1, p2, p3)
            g_SphereVertices.push_back({ p1, { 1.0f, 0.0f, 0.0f, 1.0f } });
            g_SphereVertices.push_back({ p2, { 0.0f, 1.0f, 0.0f, 1.0f } });
            g_SphereVertices.push_back({ p3, { 0.0f, 0.0f, 1.0f, 1.0f } });

            // Triangle 2 (p1, p3, p4)
            g_SphereVertices.push_back({ p1, { 1.0f, 0.0f, 0.0f, 1.0f } });
            g_SphereVertices.push_back({ p3, col });
            g_SphereVertices.push_back({ p4, col });
        }
    }
}