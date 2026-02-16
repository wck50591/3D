
// ========================================================================
//    Name : Post Effect Vertex Shader (shader_vertex_post.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/29
// ========================================================================

// 定数バッファ

struct VS_IN
{
    float3 posL : POSITION0;
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
    vo.posH = float4(vi.posL,1.0);
    vo.uv = vi.uv;
    
    return vo;
}
