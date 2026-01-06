
//Texture2D gbuffDiffuseTexture : register(t0);
//Texture2D gbuffDepthTexture   : register(t1);
//SamplerState defaultSampler   : register(s0);

//cbuffer pixelConstants : register(b0)
//{
//	float3 float3Param;
//	float floatParam;
//	int intParam;
//	int3 int3Param;
//	float4 float4Param;
//}

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(in PixelInput input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}