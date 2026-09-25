
struct SceneData
{
	float4x4 viewProjMatrix;
};

struct MeshInstanceData
{
	float4x4 modelMatrix;
};

//TODO(KL): Move these buffers to common shader data file that is shared amongst shaders
ConstantBuffer<SceneData> sceneConstants : register(b0);
StructuredBuffer<MeshInstanceData> meshInstances : register(t0);

struct VertexInput
{
	float3 position : POSITION;
	float4 color : COLOR;
	uint objectID : OBJECT_ID;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInput main(in VertexInput vertexInput)
{
	PixelInput output;

	float4 worldPos = mul(meshInstances[vertexInput.objectID].modelMatrix, float4(vertexInput.position, 1));
	output.position = mul(sceneConstants.viewProjMatrix, worldPos);
	output.color = vertexInput.color;

	return output;
}
