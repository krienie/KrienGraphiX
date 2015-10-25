
cbuffer PerPixelData : register(b0)
{
	float4 aLightClr;
	float3 dLightDir;
	float dLightIntensity;
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
	float nDotl = saturate( dot( norm, dLightDir ) );

	// calculate reflectance
	float3 h = normalize( dLightDir - camPosition );
	float specLighting = pow( saturate( dot( h, norm ) ), 128 );

	float4 finalColor = aLightClr + (copper * nDotl * dLightIntensity) + specLighting * 0.25f;
	return float4(finalColor.rgb, 1.0f);
}