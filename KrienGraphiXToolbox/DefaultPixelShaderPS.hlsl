
struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXTURE;
};


float4 main( PixelInput input ) : SV_TARGET
{
	return float4( 1.0f, 1.0f, 0.0f, 1.0f );
}