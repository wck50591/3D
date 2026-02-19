/*==============================================================================

   2d vertex draw with effect [shader_vertex_2d_effect.hlsl]
														 Author : Wai chun kit
														 Date   : 2026/02/18
--------------------------------------------------------------------------------

==============================================================================*/

// 定数バッファ
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 proj;
};

cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 world;
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
    
    float4x4 mtx = mul(world, proj);
    vo.posH = mul(vi.posL, mtx);
    
    vo.color = vi.color;
    vo.uv = vi.uv;
    
    return vo;
}
