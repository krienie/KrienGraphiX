
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

	// Texture coordinates range [0, 2], but only [0, 1] appears on screen.
	//output.texCoord = float2(uint2(VertID << 1, VertID) & 2);
	//output.position = float4(lerp(float2(-1, 1), float2(1, -1), output.texCoord), 0, 1);

	output.position = float4(vertexInput.position, 1);
	output.color = vertexInput.color;

	return output;
}