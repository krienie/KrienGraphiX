
#include "MTLTexture2D.h"

#include "MTLPixelFormat.h"
#include "MTLRenderHardwareInterface.h"

namespace
{
MTL::TextureUsage toMTLTextureUsage(const kgx::RHI::RHIResource::CreationFlags& flags)
{
	using CreationFlags = kgx::RHI::RHIResource::CreationFlags;

	MTL::TextureUsage outFlags = MTL::TextureUsageUnknown;
	outFlags |= flags & CreationFlags::RenderTargetable ? MTL::TextureUsageRenderTarget : MTL::TextureUsageUnknown;
	outFlags |= flags & CreationFlags::UnorderedAccess ? MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite : MTL::TextureUsageUnknown;
	outFlags |= flags & CreationFlags::ShaderResource ? MTL::TextureUsageShaderRead : MTL::TextureUsageUnknown;
	outFlags |= flags & CreationFlags::Atomic ? MTL::TextureUsageShaderAtomic : MTL::TextureUsageUnknown;

	return outFlags;
}
}

namespace kgx::RHI
{
MTLTexture2D::MTLTexture2D(const MTLTexture2DDescriptor& descriptor)
	: RHITexture2D(descriptor)
{
	constexpr bool isMipmapped = false;
	MTL::TextureDescriptor* desc = MTL::TextureDescriptor::texture2DDescriptor(
		toMTLPixelFormat(descriptor.pixelFormat),
		descriptor.width,
		descriptor.height,
		isMipmapped
	);

	desc->setUsage(toMTLTextureUsage(descriptor.flags));
	desc->setStorageMode(MTL::StorageModePrivate);

	mTextureResource = NS::TransferPtr(getMTLRHI()->getMTLDevice()->newTexture(desc));
}

MTL::Texture* MTLTexture2D::getNativeResource() const
{
	return mTextureResource.get();
}
}
