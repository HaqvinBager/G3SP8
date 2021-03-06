struct VertexInput
{
    unsigned int myIndex : SV_VertexID;
};

struct VertexToGeometry
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float4 myUVRect : UVRECT;
    float2 mySize : SIZE;
    float myRotation : ROTATION;
};

struct GeometryToPixel
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct PixelOutput
{
    float4 myColor : SV_TARGET;
};

cbuffer ObjectBuffer : register(b0)
{
    float4 myPosition;
    float4 myColor;
    float4 myUVRect;
    float2 mySize;
    float myRotation;
    float objectBufferPadding;
};

cbuffer TextureScrollBuffer : register(b1)
{
    float2 scrollSpeed1;
    float2 scrollSpeed2;
    float2 scrollSpeed3;
    float2 scrollSpeed4;
    
    float uvScale1;
    float uvScale2;
    float uvScale3;
    float uvScale4;
    
    float3 glowColor;
    float glowWidth;
    
    float scrollTimer;
    float level;
    bool verticalDirectionOfChange;
    float uvScale5;
    
    float2 scrollSpeed5;
    float maskOffset;
    float randomOffset;
}

Texture2D instanceTexture1 : register(t0);
Texture2D instanceTexture2 : register(t1);
Texture2D instanceTexture3 : register(t2);
Texture2D instanceTexture4 : register(t3);
Texture2D instanceTexture5 : register(t4);
Texture2D instanceTexture6 : register(t5);
SamplerState defaultSampler : register(s0);