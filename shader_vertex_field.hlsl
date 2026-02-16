// ========================================================================
//    Name : フィールド描画用頂点シェーダー (shader_vertex_field.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/10/20
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

//cbuffer VS_CONSTANT_BUFFER : register(b3)
//{
//    float4 ambient_color;
//};

//cbuffer VS_CONSTANT_BUFFER : register(b4)
//{
//    float4 directional_world_vector;
//    float4 directional_color;
//};

struct VS_IN
{
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 blend : COLOR0;
    float2 uv : TEXCOORD0;
};

struct VS_0UT
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 blend : COLOR0;
    float2 uv : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_0UT main(VS_IN vi)
{
    VS_0UT vo;
    
    // 座標変更
    
    //float4 posW = mul(vi.posL, mtxWorld);
    //float4 posWV = mul(posW, mtxView);
    //vo.posH = mul(posWV, mtxProj);
    //vo.posH = mul(vi.posL, mul(mul(mtxWorld, mtxView), mtxProj));
    
    float4x4 mtxWV = mul(mtxWorld, mtxView);
    float4x4 mtxWVP = mul(mtxWV, mtxProj);
    vo.posH = mul(vi.posL, mtxWVP);
    
    //normalW
    //ワールド変換行列の転置逆行列
    float4 normalW = mul(float4(vi.normalL.xyz, 0.0f), mtxWorld);
    vo.normalW = normalW;
    vo.posW = mul(vi.posL, mtxWorld);
    
    vo.blend = vi.blend;
    vo.uv = vi.uv;
    
    return vo;
}