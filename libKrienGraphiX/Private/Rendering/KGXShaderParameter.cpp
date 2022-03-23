
#include "Private/Rendering/KGXShaderParameter.h"

#include "Private/Core/RenderCore.h"

namespace kgx::Rendering
{
KGXShaderParameter::KGXShaderParameter(std::string name, unsigned int size, unsigned int offset, int bufferIndex, KGXShaderProgram *parentShaderProgram)
    : mName(std::move(name)), mSizeInBytes(size), mOffset(offset), mBufferIndex(bufferIndex), mParentShaderProgram(parentShaderProgram)
{
}

std::string KGXShaderParameter::getName() const
{
    return mName;
}

unsigned int KGXShaderParameter::getSizeInBytes() const
{
    return mSizeInBytes;
}

unsigned int KGXShaderParameter::getOffset() const
{
    return mOffset;
}

int KGXShaderParameter::getBufferIndex() const
{
    return mBufferIndex;
}

}
