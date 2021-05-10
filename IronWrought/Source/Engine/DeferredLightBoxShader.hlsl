#include "DeferredSamplingFunctions.hlsli"
#include "DeferredPBRFunctions.hlsli"
#include "BoxLightShaderStructs.hlsli"

PixelOutput main(BoxLightVertexToPixel input)
{
    PixelOutput output;

    float2 screenUV = (input.myUV.xy / input.myUV.z) * 0.5f + 0.5f;
    
    float depth = PixelShader_Exists(screenUV).r;
    if (depth == 1)
    {
        output.myColor = GBuffer_Albedo(screenUV);
        return output;
    }
    
    float3 worldPosition = /*input.myWorldPosition*/PixelShader_WorldPosition(screenUV).rgb;
    
    
    //float3 clipSpace = input.myClipSpacePosition;
    //clipSpace.y *= -1.0f;
    //float2 screenSpaceUV = (clipSpace.xy / clipSpace.z) * 0.5f + 0.5f;
    
    float z = depthTexture.Sample(defaultSampler, screenUV).r;
    float x = screenUV.x * 2.0f - 1;
    float y = (1 - screenUV.y) * 2.0f - 1;
    float4 projectedPos = float4(x, y, z, 1.0f);
    float4 viewSpacePos = mul(toCameraFromProjection, projectedPos);
    viewSpacePos /= viewSpacePos.w;
    float4 worldPosFromDepth = mul(toWorldFromCamera, viewSpacePos);
    
    float4 objectPosition = mul(toBoxLightObject, worldPosFromDepth);
    
    clip(0.5f - abs(objectPosition.xy));
    clip(1.0f - /*abs*/(objectPosition.z));
    
    float3 toEye = normalize(cameraPosition.xyz - worldPosition.xyz);
    float3 albedo = GBuffer_Albedo(screenUV).rgb;
    albedo = GammaToLinear(albedo);
    float3 normal = GBuffer_Normal(screenUV).xyz;
    float3 vertexNormal = GBuffer_VertexNormal(screenUV).xyz;
    float ambientOcclusion = GBuffer_AmbientOcclusion(screenUV);
    float metalness = GBuffer_Metalness(screenUV);
    float perceptualRoughness = GBuffer_PerceptualRoughness(screenUV);
    float emissiveData = GBuffer_Emissive(screenUV);
    
    float3 specularColor = lerp((float3) 0.04, albedo, metalness);
    float3 diffuseColor = lerp((float3) 0.00, albedo, 1 - metalness);
    
    float3 toLight = boxLightPositionAndRange.xyz - worldPosition.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);
    float3 directionalLight = EvaluateBoxLight(diffuseColor, specularColor, normal, perceptualRoughness, boxLightColorAndIntensity.rgb * boxLightColorAndIntensity.w, boxLightPositionAndRange.w, toLight, lightDistance, toEye, boxLightDirection.xyz);
    float3 radiance = directionalLight * (1.0f - ShadowFactor(worldPosition, boxLightPositionAndRange.xyz, toBoxLightView, toBoxLightProjection, shadowDepthTexture, shadowSampler, boxLightShadowmapResolution));

    output.myColor.rgb = radiance;
    output.myColor.a = 1.0f;
    return output;
}