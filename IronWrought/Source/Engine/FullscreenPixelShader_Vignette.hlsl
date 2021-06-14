#include "FullscreenShaderStructs.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float4 resource = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).rgba;
    float4 vignetteTexture = fullscreenTexture2.Sample(defaultSampler, input.myUV.xy).rgba;
    
    float2 uv = input.myUV.xy;
    uv -= 0.5f;
    uv *= 2.0f;
    float vignetteStrength = 1.0f / 9.0f; // 1/4 to 1/16
    float vignette = (uv.x * uv.x + uv.y * uv.y) * myVignetteStrength;
    
    vignette = cos(vignette * (3.14159265f * 2.0f));
    
    //resource.x *= clamp(vignette + myVignetteColor.r, 0.0f, 1.0f);
    //resource.y *= clamp(vignette + myVignetteColor.g, 0.0f, 1.0f);
    //resource.z *= clamp(vignette + myVignetteColor.b, 0.0f, 1.0f);
    
    resource = lerp(vignetteTexture, resource, saturate(vignette));
    
    returnValue.myColor.rgb = resource.rgb;
    returnValue.myColor.a = resource.a;
    
    return returnValue;
}