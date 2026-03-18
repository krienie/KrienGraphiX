
#pragma once

#include <memory>
#include <Foundation/NSSharedPtr.hpp>
#include <Metal/MTLTexture.hpp>

#include "MTLDescriptors.h"

#include "Private/RHI/RHITexture2D.h"
#include "Private/RHI/RHIUtils.h"

namespace kgx::RHI
{
class MTLTexture2D : public RHITexture2D
{
public:
	MTLTexture2D(const MTLTexture2DDescriptor& descriptor);
	~MTLTexture2D() override = default;

	[[nodiscard]] MTL::Texture* getNativeResource() const;

private:
	NS::SharedPtr<MTL::Texture> mTextureResource;
};

DEFINE_RESOURCE_CAST(MTLTexture2D, RHITexture2D)
DEFINE_RESOURCE_CAST(MTLTexture2D, RHIViewableResource)
}

