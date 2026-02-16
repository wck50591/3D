
// ========================================================================
//    Name : ビルボード描画用頂点シェーダー (shader_vertex_billboard.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/14
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

cbuffer VS_CONSTANT_BUFFER : register(b3)
{
    float2 scale;
    float2 translation;
};

struct VS_IN
{
    float4 posL : POSITION0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct VS_0UT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
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
    
    vo.color = vi.color;
    vo.uv = vi.uv * scale + translation;
    
    return vo;
}
