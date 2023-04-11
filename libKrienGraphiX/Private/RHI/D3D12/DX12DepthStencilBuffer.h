
#pragma once

#include "Private/RHI/RHIDepthStencilBuffer.h"
#include "DX12Texture2D.h"

#include <D3D12.h>
#include <memory>
#include <wrl\client.h>

namespace kgx::RHI
{
class DX12DepthStencilBuffer : public RHIDepthStencilBuffer
{
    public:
        DX12DepthStencilBuffer(DX12GraphicsDevice* dxDevice, const RHITexture2DDescriptor& descriptor);
        ~DX12DepthStencilBuffer() override = default;
        
        [[nodiscard]] D3D12_RESOURCE_STATES getCurrentState() const;
        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> getResource() const;

        [[nodiscard]] virtual RHIResourceView* getDepthStencilView() const;

private:
        std::shared_ptr<DX12Texture2D> mTexture;
};
}
