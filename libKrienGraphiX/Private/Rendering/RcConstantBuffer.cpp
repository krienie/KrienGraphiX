
#include "Private/Rendering/RcConstantBuffer.h"
#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
RcConstantBuffer::RcConstantBuffer(const ConstantBufferDescriptor & descriptor)
    : mRHIConstantBuffer(nullptr)
{
    mRHIConstantBuffer = core::RenderCore::get()->getRHI()->createConstantBuffer(descriptor.name, descriptor.bufferRegister, bufferSize);
}

std::string RcConstantBuffer::name() const
{
    //TODO: make sure mRHIConstantBuffer is always available
    return mRHIConstantBuffer->name();
}

}
