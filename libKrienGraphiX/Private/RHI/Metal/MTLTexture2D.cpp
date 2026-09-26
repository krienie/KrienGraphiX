
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
	//TODO(KL): Implement other texture types
	desc->setTextureType(MTL::TextureType2D);
	desc->setSampleCount(1);

	MTL::Device* mtlDevice = getMTLRHI()->getMTLDevice()->getNativeDevice();
	mTextureResource = NS::TransferPtr(mtlDevice->newTexture(desc));

	mIsMultisampled = descriptor.numSamples > 1;
	if (mIsMultisampled)
	{
		MTL::TextureDescriptor* multisampleDesc = MTL::TextureDescriptor::texture2DDescriptor(
			toMTLPixelFormat(descriptor.pixelFormat),
			descriptor.width,
			descriptor.height,
			isMipmapped
		);

		multisampleDesc->setUsage(toMTLTextureUsage(descriptor.flags));
		multisampleDesc->setStorageMode(MTL::StorageModeMemoryless);
		multisampleDesc->setTextureType(MTL::TextureType2DMultisample);
		multisampleDesc->setSampleCount(descriptor.numSamples);

		mMultisampledTextureResource = NS::TransferPtr(mtlDevice->newTexture(multisampleDesc));
	}

	//TODO(KL): For now everything is permanently resident.
	//Will change for a different system later when scene organisation is more developed.
	MTLResidencyManager::addGlobalResidency(mTextureResource.get());
}

MTLTexture2D::~MTLTexture2D()
{
	MTLResidencyManager::removeGlobalResidency(mTextureResource.get());
}

void* MTLTexture2D::getNativeResource() const
{
	return mTextureResource.get();
}

MTL::Texture* MTLTexture2D::getTextureResource() const
{
	return mTextureResource.get();
}

MTL::Texture* MTLTexture2D::getMultisampledTextureResource() const
{
	return mMultisampledTextureResource.get();
}

bool MTLTexture2D::isMultisampled() const
{
	return mIsMultisampled;
}
}
