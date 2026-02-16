
// ========================================================================
//    Name : 3D(ライトなし)描画用頂点シェーダー (shader_vertex_3d_unlit.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/21
// ========================================================================

// 定数バッファ
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 mtxWorld;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 mtxView;
};

cbuffer VS_CONSTANT_BUFFER : register(b2)
{
    float4x4 mtxProj;
};

struct VS_IN
{
    float4 posL : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VS_0UT
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_0UT main(VS_IN vi)
{
    VS_0UT vo;
    
    // 座標変更
    
    float4x4 mtxWV = mul(mtxWorld, mtxView);
    float4x4 mtxWVP = mul(mtxWV, mtxProj);
    vo.posH = mul(vi.posL, mtxWVP);
    
    vo.uv = vi.uv;
    
    return vo;
}
