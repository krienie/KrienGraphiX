
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 normal   : NORMAL;
	float2 texCoord : TEXTURE;
};

float4 main( PixelInput input ) : SV_TARGET
{
	//TODO: write to GBuffers
	return float4(0.0f, 1.0f, 1.0f, 1.0f);
}