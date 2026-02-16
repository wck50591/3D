
// ========================================================================
//    Name : ビルボード描画用ピクセルシェーダー (shader_pixel_billboard.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/11/14
// ========================================================================

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
    float4 color = tex.Sample(samp, pi.uv) * pi.color;
    
    if (color.a < 0.5f)
    {
        discard;
    }
    //clip(sin(pi.uv.x * 1000));
    //if (color.a < 0.6f)
    //{
    //    discard;
    //}
        return color;
    //return tex.Sample(samp, pi.uv) * pi.color;
}
