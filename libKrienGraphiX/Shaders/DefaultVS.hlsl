
cbuffer MainConstants : register(b0)
{
	float4x4 modelMatrix;
	float4x4 viewProjMatrix;
}

struct VertexInput
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInput main(in VertexInput vertexInput)
{
	PixelInput output;

	float4x4 mvp = mul(viewProjMatrix, modelMatrix);
	output.position = mul(mvp, float4(vertexInput.position, 1));
	output.color = vertexInput.color;

	return output;
}