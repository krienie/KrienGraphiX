
#include "Private/Rendering/RcShaderParameter.h"

#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
RcShaderParameter::RcShaderParameter(std::string name, unsigned int size, unsigned int offset, int bufferIndex, RcShaderProgram *parentShaderProgram)
    : mName(std::move(name)), mSizeInBytes(size), mOffset(offset), mBufferIndex(bufferIndex), mParentShaderProgram(parentShaderProgram)
{
}

std::string RcShaderParameter::getName() const
{
    return mName;
}

unsigned int RcShaderParameter::getSizeInBytes() const
{
    return mSizeInBytes;
}

unsigned int RcShaderParameter::getOffset() const
{
    return mOffset;
}

int RcShaderParameter::getBufferIndex() const
{
    return mBufferIndex;
}

}
