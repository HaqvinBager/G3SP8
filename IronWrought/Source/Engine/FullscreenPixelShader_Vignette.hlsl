#include "FullscreenShaderStructs.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float4 resource = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).rgba;

    float2 uv = input.myUV.xy;
    uv -= 0.5f;
    uv *= 2.0f;
    float vignetteStrength = 1.0f / 9.0f; // 1/4 to 1/16
    float vignette = (uv.x * uv.x + uv.y * uv.y) * vignetteStrength;

    vignette = cos(vignette * (3.14159265f * 2.0f));
    
    float3 vignetteColor = float3(0.0f, 0.0f, 0.0f);

    resource.x *= clamp(vignette + vignetteColor.r, 0.0f, 1.0f);
    resource.y *= clamp(vignette + vignetteColor.g, 0.0f, 1.0f);
    resource.z *= clamp(vignette + vignetteColor.b, 0.0f, 1.0f);
    
    returnValue.myColor.rgb = resource.rgb;
    returnValue.myColor.a = 1.0f;
    
    return returnValue;
}