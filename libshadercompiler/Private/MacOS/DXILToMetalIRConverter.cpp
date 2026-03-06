
#include "DXILToMetalIRConverter.h"

#ifdef __APPLE__
#include <metal_irconverter/metal_irconverter.h>
#endif

namespace kgx::DXILToMetalIRConverter
{
#ifdef __APPLE__
std::vector<char> convertToMetalIR(const std::vector<char>& dxilByteCode, const std::string& entryPoint)
{
    std::vector<char> metalIrOut;

    IRCompiler* pCompiler = IRCompilerCreate();
    IRCompilerSetEntryPointName(pCompiler, entryPoint.c_str());

    IRObject* pDXIL = IRObjectCreateFromDXIL((uint8_t*)dxilByteCode.data(), dxilByteCode.size(), IRBytecodeOwnershipNone);

    // Compile DXIL to Metal IR:
    IRError* pError = nullptr;
    IRObject* pOutIR = IRCompilerAllocCompileAndLink(pCompiler, NULL,  pDXIL, &pError);

    if (!pOutIR)
    {
        //TODO(KL): Do something with the error
        IRErrorDestroy(pError);
        return metalIrOut;
    }

    const IRShaderStage shaderStage = IRObjectGetMetalIRShaderStage(pOutIR);

    IRMetalLibBinary* pMetallib = IRMetalLibBinaryCreate();
    IRObjectGetMetalLibBinary(pOutIR, shaderStage, pMetallib);
    const size_t metallibSize = IRMetalLibGetBytecodeSize(pMetallib);

    metalIrOut.resize(metallibSize);
    IRMetalLibGetBytecode(pMetallib, (uint8_t*)metalIrOut.data());

    IRMetalLibBinaryDestroy(pMetallib);
    IRObjectDestroy(pDXIL);
    IRObjectDestroy(pOutIR);
    IRCompilerDestroy(pCompiler);

    return metalIrOut;
}
#endif
}
