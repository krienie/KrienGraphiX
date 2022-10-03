
#include "DX12Texture2D.h"

#include <cassert>

#include "DX12GraphicsDevice.h"
#include "DX12Descriptors.h"
#include "DX12PixelFormat.h"
#include "Private/Core/RenderCore.h"

namespace kgx::RHI
{
DX12Texture2D::DX12Texture2D(DX12GraphicsDevice* dxDevice, const DX12Texture2DDescriptor& descriptor)
    : RHITexture2D(descriptor), DX12Resource(descriptor.textureResource, nullptr, descriptor.initialState)
{
    // Create the texture resource if it does not exist yet using the information in the passed texture descriptor
    if (!descriptor.textureResource)
    {
        //TODO(KL): Create constructor for placed texture resource

        ID3D12Device * nativeDevice = dxDevice->getNativeDevice();

        const DXGI_FORMAT dxPixelFormat = toDxgiPixelFormat(descriptor.pixelFormat);

        //TODO(KL): Set sample quality

        const D3D12_CLEAR_VALUE clearValue = toDx12ClearValue(descriptor.pixelFormat, descriptor.clearValue);
        const D3D12_RESOURCE_FLAGS resourceFlags = toDx12ResourceFlags(descriptor.flags);

        // Create the actual texture resource, including a heap for it to live in
        HRESULT res = nativeDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(dxPixelFormat, width(), height(), 1, static_cast<UINT16>(numMips()), numSamples(), 0, resourceFlags),
            descriptor.initialState,
            &clearValue,
            IID_PPV_ARGS(&mResource));
    }
}
}
