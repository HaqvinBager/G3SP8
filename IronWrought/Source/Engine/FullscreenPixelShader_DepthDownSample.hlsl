#include "FullscreenShaderStructs.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    //float4 depth2x2 = fullscreenTexture1.Load(float3(input.myUV.xy, 0));
    //returnValue.myColor = depth2x2;
    float4 depth2x2 = fullscreenTexture1.GatherRed(defaultSampler, input.myUV.xy);
    float maxDepth = max(depth2x2.x, depth2x2.y);
    maxDepth = max(maxDepth, depth2x2.z);
    maxDepth = max(maxDepth, depth2x2.w);
    returnValue.myColor.r = maxDepth;
    returnValue.myColor.a = 1.0f;
    return returnValue;
};