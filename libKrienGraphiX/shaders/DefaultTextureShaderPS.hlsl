
struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXTURE;
};

Texture2D tex;
SamplerState samp;


float4 main( PixelInput input ) : SV_TARGET
{
	return tex.Sample( samp, input.texCoord );
}
