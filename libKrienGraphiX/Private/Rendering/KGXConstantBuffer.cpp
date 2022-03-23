
#include "Private/Rendering/KGXConstantBuffer.h"
#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
//TODO(KL): Temporary maybe_unused
KGXConstantBuffer::KGXConstantBuffer([[maybe_unused]] const ConstantBufferDescriptor & descriptor)
    : mRHIConstantBuffer(nullptr)
{
    //auto * renderEngine = core::RenderCore::get();
    //mRHIConstantBuffer = core::RenderCore::get()->getRHI()->createConstantBuffer(renderEngine->getGraphicsDevicePtr(), descriptor);
}

std::string KGXConstantBuffer::name() const
{
    //TODO: make sure mRHIConstantBuffer is always available
    return mRHIConstantBuffer->name();
}

}
