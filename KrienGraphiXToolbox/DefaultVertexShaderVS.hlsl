

cbuffer PerCameraData : register(b0)
{
	matrix<float,4,4> viewMatrix;
	matrix<float,4,4> projMatrix;
	//float3 cameraPos;
};


cbuffer PerObjectData : register(b1)
{
	matrix<float,4,4> modelMatrix;
};


struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXTURE;
};


PixelInput main( float4 pos : POSITION, float3 tex : TEXCOORD, float3 norm : NORMAL )
{
	float4 newPos = mul( modelMatrix, pos );
	newPos = mul( viewMatrix, newPos );
	newPos = mul( projMatrix, newPos );

	PixelInput output;
	output.position = newPos;
	output.texCoord = tex.xy;

	return output;
}