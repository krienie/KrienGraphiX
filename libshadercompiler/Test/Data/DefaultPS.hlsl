
//Texture2D gbuffDiffuseTexture : register(t0);
//Texture2D gbuffDepthTexture   : register(t1);
//SamplerState defaultSampler   : register(s0);

cbuffer pixelConstants : register(b0)
{
    float3 float3Param;
    float floatParam;
    int intParam;
    int3 int3Param;
    float4 float4Param;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXTURE;
};

float4 main(PixelInput input) : SV_TARGET
{
    float dummy = float3Param.x
                    + float3Param.y
                    + float3Param.z
                    + floatParam
                    + float(intParam)
                    + float(int3Param.x)
                    + float(int3Param.y)
                    + float(int3Param.z)
                    + float4Param.x * float4Param.y * float4Param.z * float4Param.w;
    return float4(dummy, 1.0f, 0.0f, 1.0f);
}