
cbuffer SceneConstants : register(b0)
{
	column_major float4x4 viewProjMatrix;
}

cbuffer InstanceConstants : register(b1)
{
	column_major float4x4 modelMatrix;
}

//Texture2D gbuffDiffuseTexture : register(t0);
//Texture2D gbuffDepthTexture   : register(t1);
//SamplerState defaultSampler   : register(s0);

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 main(in PixelInput input) : SV_TARGET
{
	return input.color;
}