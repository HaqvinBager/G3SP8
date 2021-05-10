#include "DeferredShaderStructs.hlsli"

VertexModelToPixel main(VertexModelInput input)
{
    VertexModelToPixel returnValue;

    float4 vertexObjectPos      = input.myPosition.xyzw;
    float4 vertexWorldPos       = mul(input.myTransform, vertexObjectPos);
    float4 vertexViewPos        = mul(toCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos  = mul(toProjectionSpace, vertexViewPos);
    
    float3x3 toWorldRotation = (float3x3) input.myTransform;
    float3 vertexWorldNormal    = mul(toWorldRotation, input.myNormal.xyz);
    float3 vertexWorldTangent   = mul(toWorldRotation, input.myTangent.xyz);
    float3 vertexWorldBinormal = mul(toWorldRotation, input.myBiTangent.xyz);

    returnValue.myPosition      = vertexProjectionPos;
    returnValue.myWorldPosition = vertexWorldPos;
    returnValue.myNormal        = float4(vertexWorldNormal, 0);
    returnValue.myTangent       = float4(vertexWorldTangent, 0);
    returnValue.myBinormal      = float4(vertexWorldBinormal, 0);
    returnValue.myUV            = input.myUV;
    return returnValue;
}