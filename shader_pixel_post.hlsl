
// ========================================================================
//    Name : Post Effect Pixel Shader (shader_pixel_post.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2026/01/29
// ========================================================================

// 定数バッファ
cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 color;
    float ivColor;
    float gamma;
    float2 dum;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D tex : register(t0); //テクスチャ
SamplerState samp : register(s0); //テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET
{
    //color
    float3 c = tex.Sample(samp, pi.uv).rgb;
    c = pow(c, gamma);
    c = lerp(c, 1.0 - c, ivColor);
    return float4(c, 1.0);
    
}
