
#pragma once

namespace kgx::RHI
{
class RHIResource
{
public:
    enum CreationFlags
    {
        None = 0,
        RenderTargetable = 1 << 1,
        UnorderedAccess = 1 << 2,
        ShaderResource = 1 << 3,
        DepthStencil = 1 << 4
    };

    // Based on D3D12_RESOURCE_STATES
    //enum class ResourceStates
    //{
    //    Common	= 0,
    //    VertexAndConstantBuffer = 0x1,
    //    IndexBuffer = 0x2,
    //    RenderTarget = 0x4,
    //    UnorderedAccess = 0x8,
    //    DepthWrite = 0x10,
    //    DepthRead = 0x20,
    //    NonPixelShaderResource = 0x40,
    //    PixelShaderResource = 0x80,
    //    StreamOut = 0x100,
    //    IndirectArgument = 0x200,
    //    CopyDestination = 0x400,
    //    CopySource = 0x800,
    //    ResolveDestination = 0x1000,
    //    ResolveSource = 0x2000,
    //    RaytracingAccelerationStructure = 0x400000,
    //    ShadingRateSource = 0x1000000,
    //    GenericRead = ( ( ( ( ( 0x1 | 0x2 )  | 0x40 )  | 0x80 )  | 0x200 )  | 0x800 ) ,
    //    AllShaderResource = ( 0x40 | 0x80 ) ,
    //    Present	= 0,
    //    Prediction = 0x200,
    //    VideoDecodeRead = 0x10000,
    //    VideoDecodeWrite = 0x20000,
    //    VideoProcessRead = 0x40000,
    //    VideoProcessWrite = 0x80000,
    //    VideoEncodeRead = 0x200000,
    //    VideoEncodeWrite = 0x800000
    //};

    explicit RHIResource(CreationFlags flags)
        : mCreationFlags(flags)
    {}
    virtual ~RHIResource() = default;

    [[nodiscard]] CreationFlags getCreationFlags() const { return mCreationFlags; }

    virtual bool IsBuffer() const { return false; }
    virtual bool IsTexture() const { return false; }

private:
    CreationFlags mCreationFlags;
};

}
