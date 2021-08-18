
#pragma once

#include <string>

namespace kgx::Rendering
{
    class RcShaderProgram;

    class RcShaderParameter final
{
    public:
        RcShaderParameter(std::string name, unsigned int size, unsigned int offset, int bufferIndex, RcShaderProgram *parentShaderProgram);
        ~RcShaderParameter() = default;

        [[nodiscard]] std::string getName() const;
        [[nodiscard]] unsigned int getSizeInBytes() const;
        [[nodiscard]] unsigned int getOffset() const;
        [[nodiscard]] int getBufferIndex() const;

    private:
        std::string mName;
        unsigned int mSizeInBytes;
        unsigned int mOffset;
        int mBufferIndex;
        RcShaderProgram *mParentShaderProgram;
};
}
