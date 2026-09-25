
struct SceneData
{
	column_major float4x4 viewProjMatrix;
};

struct MeshInstanceData
{
	column_major float4x4 modelMatrix;
};

//TODO(KL): Move these buffers to common shader data file that is shared amongst shaders
ConstantBuffer<SceneData> SceneConstants : register(b0);
StructuredBuffer<MeshInstanceData> meshInstances : register(t0);

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