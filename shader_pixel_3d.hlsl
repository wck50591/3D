
// ========================================================================
//    Name : 3D描画用ピクセルシェーダー (shader_pixel_3d.hlsl)
//
//                                                    Author : wai chunkit
//                                                    Date   : 2025/09/10
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
    float4 directional_color;
};

cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eye_PosW;
    float specular_power;
    float4 specular_color;
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

cbuffer PS_CONSTANT_BUFFER : register(b5)
{
    float4 emissive_color; // rgb = glow color, a unused
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex; //テクスチャ
SamplerState samp; //テクスチャサンプラ


float4 main(PS_IN pi) : SV_TARGET
{
    //color
    float3 material_color = tex.Sample(samp, pi.uv).rgb *
    pi.color.rgb * diffuse_color.rgb;
    
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
    
    float alpha = tex.Sample(samp, pi.uv).a * pi.color.a * diffuse_color.a;
    
    float3 color = ambient + diffuse + specular;
    
    //rim lighting
    float lim = 1.0f - max(dot(normalW.xyz, toEye), 0.0f);
    lim = pow(lim, 5.0f);
    //color += float3(lim,lim,lim);
    
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
        float3 r = reflect(normalize(lightToPixel),normalW.xyz);
        float t = pow(max(dot(r, toEye), 0.0f), specular_power);
        
        color += pointLight[i].color.rgb * t;
        
    }
    
    color += emissive_color.rgb * emissive_color.a;
    
    return float4(color, alpha);
    //float3 lcolor = pi.color.rgb * directional_color.rgb * dl
    //+ ambient_color.rgb * pi.color.rgb;
    //lcolor += float3(1.0f, 1.0f, 1.0f) * t;
    
    //return tex.Sample(samp, pi.uv) * pi.color;
    //return tex.Sample(samp, pi.uv) * pi.color * color;
    //return tex.Sample(samp, pi.uv) * float4(lcolor,1.0f) * color;
}
