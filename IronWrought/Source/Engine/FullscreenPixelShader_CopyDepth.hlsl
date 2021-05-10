#include "FullscreenShaderStructs.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float resource = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).r;
    returnValue.myColor.r = resource;
    returnValue.myColor.a = 1.0f;
    return returnValue;
}