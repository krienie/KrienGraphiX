
Texture2D gbuffDiffuseTexture : register(t0);
Texture2D gbuffDepthTexture   : register(t1);
SamplerState defaultSampler   : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXTURE;
};

float4 main( PixelInput input ) : SV_TARGET
{
    return gbuffDiffuseTexture.Sample( defaultSampler, input.texCoord );
}