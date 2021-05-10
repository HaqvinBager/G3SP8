#include "PBRAmbience.hlsli"
#include "PBRDirectionalLight.hlsli"

PixelOutPut main(VertexToPixel input)
{
    PixelOutPut output;
   
    float4 albedo = PixelShader_Color(input).myColor.rgba;
    output.myColor = albedo;
    
    return output;
}