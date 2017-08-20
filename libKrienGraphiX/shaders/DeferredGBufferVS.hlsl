
cbuffer PerCameraData : register(b0)
{
	matrix<float, 4, 4> kgx_viewMatrix;
	matrix<float, 4, 4> kgx_projMatrix;
	//float3 cameraPos;
};


cbuffer PerObjectData : register(b1)
{
	matrix<float, 4, 4> kgx_modelMatrix;
	matrix<float, 4, 4> kgx_normalMatrix;
};


struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXTURE;
	//float3 lightDir : LIGHTDIR;
	//float3 camDir   : CAMDIR;
};

PixelInput main( float4 pos : POSITION, float3 tex : TEXCOORD, float3 norm : NORMAL )
{
	float4 newPos = mul( kgx_modelMatrix, pos );
	newPos = mul( kgx_viewMatrix, newPos );
	newPos = mul( kgx_projMatrix, newPos );

	PixelInput output;
	output.position = newPos;
	output.texCoord = tex.xy;
	output.normal = mul( float4(norm, 0.0f), kgx_normalMatrix );

	return output;
}