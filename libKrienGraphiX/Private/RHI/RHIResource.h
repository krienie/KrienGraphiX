
#pragma once

namespace kgx::RHI
{
class RHIResource
{
public:
	enum CreationFlags : int
	{
		None = 0,
		RenderTargetable = 1 << 1,
		UnorderedAccess = 1 << 2,
		ShaderResource = 1 << 3,
		DepthStencil = 1 << 4,
		IndexBuffer = 1 << 5,
		VertexBuffer = 1 << 6,
		ConstantBuffer = 1 << 7,
		Atomic = 1 << 8 // Metal only
	};

	explicit RHIResource(CreationFlags flags)
		: mCreationFlags(flags)
	{}
	virtual ~RHIResource() = default;

	[[nodiscard]] CreationFlags getCreationFlags() const { return mCreationFlags; }

	[[nodiscard]] virtual bool IsBuffer() const { return false; }
	[[nodiscard]] virtual bool IsTexture() const { return false; }

	[[nodiscard]] virtual void* getNativeResource() const = 0;

private:
	CreationFlags mCreationFlags;
};

}
