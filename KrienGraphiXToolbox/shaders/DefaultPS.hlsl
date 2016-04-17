
struct Light
{
	float3 direction;
	float intensity;
};

cbuffer PerPixelData : register(b0)
{
	Light lights;
	float4 ambLightClr;
	float4 kgx_diffuse;
	float4 kgx_specular;
	float3 camPosition;
	float padding;
};

Texture2D diffuseTexture    : register(t0);
SamplerState DefaultSampler : register(s0);

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXTURE;
};

float4 main( PixelInput input ) : SV_TARGET
{
	// calculate lighting
	float3 norm = normalize( input.normal ).xyz;
	float nDotl = saturate( dot( norm, lights.direction ) );

	// calculate reflectance
	float3 h = normalize( lights.direction - camPosition );
	float specLighting = pow( saturate( dot( h, norm ) ), 128 );

	float4 diffuseClr = diffuseTexture.Sample( DefaultSampler, input.texCoord );
	float4 finalColor = ambLightClr + (diffuseClr * nDotl * lights.intensity) + (specLighting * 0.25f) * kgx_specular;
	return float4(finalColor.rgb, 1.0f);
}