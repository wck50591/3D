
// ========================================================================
//    Name : フィールド用ピクセルシェーダー (shader_pixel_field.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/26
// ========================================================================

// 定数バッファ
cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 diffuse_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b1)
{
    float4 ambient_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b2)
{
    float4 directional_world_vector;
    float4 directional_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};

cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eye_PosW;
    float specular_power = 30.0f;
    float4 specular_color = { 0.1f, 0.1f, 0.1f, 0.1f };
};

struct PointLight
{
    float3 posW;
    float range;
    float4 color;
};

cbuffer PS_CONSTANT_BUFFER : register(b4)
{
    PointLight pointLight[4];
    int pointLight_count;
    float3 pointLight_dummy;
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 blend : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex0 : register(t0); //テクスチャ
Texture2D tex1 : register(t1); //テクスチャ

SamplerState samp; //テクスチャサンプラ


float4 main(PS_IN pi) : SV_TARGET
{
    //float2 uv = pi.uv * 2.0f;
    
    //uv edit - rotation
    float2 uv;
    float deg = 3.14159 * 45 / 180.0f;
    uv.x = pi.uv.x * cos(deg) + pi.uv.y * sin(deg);
    uv.y = -pi.uv.x * sin(deg) + pi.uv.y * cos(deg);
    
    // blend 2 pic 
    float4 tex_color = tex0.Sample(samp, pi.uv) * pi.blend.g 
    + tex1.Sample(samp, pi.uv) * pi.blend.r;
    
    //材質の色
    float3 material_color = tex_color.rgb * diffuse_color.rgb;
    
    //並行光源
    float4 normalW = normalize(pi.normalW);
    //float dl = max(0, dot(-directional_world_vector, normalW)); // <90 
    float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f; // 
    float3 diffuse = material_color * directional_color.rgb * dl;
    
    //環境光
    float3 ambient = material_color * ambient_color.rgb;
    //float3 ambient = {0.0f,0.0f,0.0f };
    
    //スペキュラ
    float3 toEye = normalize(eye_PosW - pi.posW.xyz);
    float3 r = reflect(directional_world_vector, normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), specular_power);
    float3 specular = specular_color.rgb * t;
    //return tex_color * pi.directional * pi.ambient;
    
    //
    float3 color = ambient + diffuse + specular;
    
    //point light
    for (int i = 0; i < pointLight_count; i++)
    {
        //point light to pixel vector
        float3 lightToPixel = pi.posW.xyz - pointLight[i].posW;
        
        //面(ピクセル)とライトとの距離を測る
        float pointLightLength = length(lightToPixel);
    
        //影響力の計算
        float A = pow(max(1.0f - 1.0f / pointLight[i].range * pointLightLength, 0.0f), 2.0f);
        
        //point light and object pixel direction
        float dl = max(0.0f, dot(-normalize(lightToPixel), normalW.xyz)); // <90 
        //float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f; // 
        
        color += material_color * pointLight[i].color.rgb * A * dl;
    
        //point light specular
        float3 r = reflect(normalize(lightToPixel), normalW.xyz);
        float t = pow(max(dot(r, toEye), 0.0f), specular_power);
        
        color += pointLight[i].color.rgb * t;
    }
    
    return float4(color, 1.0f);
    //return tex.Sample(samp, pi.uv) * pi.color;
    //return tex0.Sample(samp, pi.uv * 0.1f) * 0.5f + tex1.Sample(samp, uv) * 0.5f; // * pi.color;
}
