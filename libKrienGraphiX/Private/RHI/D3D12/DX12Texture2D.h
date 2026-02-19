
#pragma once

#include "Private/RHI/RHITexture2D.h"

#include "DX12Resource.h"

namespace kgx::RHI
{
class DX12GraphicsDevice;
struct DX12Texture2DDescriptor;

class DX12Texture2D : public RHITexture2D, public DX12Resource
{
public:
	DX12Texture2D(const DX12Texture2DDescriptor& descriptor);
	~DX12Texture2D() override = default;
};

DEFINE_RESOURCE_CAST(DX12Texture2D, RHITexture2D)
}
