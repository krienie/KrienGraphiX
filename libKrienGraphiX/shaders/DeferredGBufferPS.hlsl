
cbuffer PerObjectData : register(b0)
{
    float4 kgx_diffuse;
    float4 kgx_specular;
    bool   kgx_useTexture;
    float3 padding_PerObjectData;
};

Texture2D diffuseTexture    : register(t0);
SamplerState defaultSampler : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texCoord : TEXTURE;
};

struct PixelOutput
{
    float4 diffuse : SV_TARGET0;
    float4 depth   : SV_TARGET1;
};

PixelOutput main( PixelInput input )
{
    PixelOutput output;
    output.diffuse = kgx_useTexture ? diffuseTexture.Sample(defaultSampler, input.texCoord) : kgx_diffuse;
    float curDepth = input.position.z / input.position.w;
    output.depth = float4(curDepth, curDepth, curDepth, 1.0);

    return output;
}