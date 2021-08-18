
struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXTURE;
};

PixelInput main(in uint VertID : SV_VertexID)
{
    PixelInput output;

    // Texture coordinates range [0, 2], but only [0, 1] appears on screen.
    output.texCoord = float2(uint2(VertID << 1, VertID) & 2);
    output.position = float4(lerp(float2(-1, 1), float2(1, -1), output.texCoord), 0, 1);

    return output;
}