
struct Light
{
	float3 direction;
	float intensity;
};

cbuffer LightData : register(b0)
{
	float4 ambLightClr;
	Light sunLight;
};

cbuffer PerFrameData : register(b1)
{
	float3 camPosition;
	float padding;
};

cbuffer PerObjectData : register(b2)
{
	float4 kgx_diffuse;
	float4 kgx_specular;
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
	float nDotl = saturate( dot( norm, sunLight.direction ) );

	// calculate reflectance
	float3 h = normalize( sunLight.direction - camPosition );
	float specLighting = pow( saturate( dot( h, norm ) ), 128 );

	float4 diffuseClr = diffuseTexture.Sample( DefaultSampler, input.texCoord );
	float4 finalColor = ambLightClr + (diffuseClr * nDotl * sunLight.intensity) + (specLighting * 0.25f) * kgx_specular;
	return float4(finalColor.rgb, 1.0f);
}