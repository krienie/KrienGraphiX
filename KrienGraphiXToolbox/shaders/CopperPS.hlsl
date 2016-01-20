
struct Light
{
	float3 direction;
	float intensity;
};

cbuffer PerPixelData : register(b0)
{
	Light lights;
	float4 ambLightClr;
	float3 camPosition;
	float padding;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXTURE;
};

float4 main( PixelInput input ) : SV_TARGET
{
	float4 copper = float4(0.8f, 0.3f, 0.1f, 1.0f);

	// calculate lighting
	float3 norm = normalize( input.normal ).xyz;
	float nDotl = saturate( dot( norm, lights.direction ) );

	// calculate reflectance
	float3 h = normalize( lights.direction - camPosition );
	float specLighting = pow( saturate( dot( h, norm ) ), 128 );

	float4 finalColor = ambLightClr + (copper * nDotl * lights.intensity) + specLighting * 0.25f;
	return float4(finalColor.rgb, 1.0f);
}