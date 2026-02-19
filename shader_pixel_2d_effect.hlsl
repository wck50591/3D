/*==============================================================================

   2d pixel draw with effect [shader_pixel_2d_effect.hlsl]
														 Author : Wai chun kit
														 Date   : 2026/02/18
--------------------------------------------------------------------------------

==============================================================================*/
cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float time;
    float intensity;
    float sliceHeight;
    float dum;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex; //テクスチャ
SamplerState samp; //テクスチャサンプラ

float4 main(PS_IN pi) : SV_TARGET
{
    //float2 uv = pi.uv;
    //// pic uv y 100..target y will become black
    //float y = 100;
    
    //for (float i = y; i < y + sliceHeight; i++)
    //{
    //    pi.color = (0.0f, 0.0f, 0.0f, 1.0f);
    //}
    
    return tex.Sample(samp, pi.uv) * pi.color;
}
