
#include "DXILToMetalIRConverter.h"

#include "ShaderCompiler/ShaderCompiler.h"

#ifdef __APPLE__
#include <metal_irconverter.h>
#endif

namespace kgx::DXILToMetalIRConverter
{
#ifdef __APPLE__
bool convertToMetalIR(const std::vector<char>& dxilByteCode, const std::string& entryPoint, CompiledShader& outShader)
{
	IRCompiler* irCompiler = IRCompilerCreate();
	IRCompilerSetEntryPointName(irCompiler, entryPoint.c_str());

	IRCompilerSetStageInGenerationMode(irCompiler, IRStageInCodeGenerationModeUseMetalVertexFetch);

	IRObject* dxilObj = IRObjectCreateFromDXIL((uint8_t*)dxilByteCode.data(), dxilByteCode.size(), IRBytecodeOwnershipNone);

	IRError* error = nullptr;
	IRObject* mtlIRObj = IRCompilerAllocCompileAndLink(irCompiler, entryPoint.c_str(), dxilObj, &error);

	if (!mtlIRObj)
	{
		//TODO(KL): Do something with the error
		IRErrorDestroy(error);
		return false;
	}

	const IRShaderStage shaderStage = IRObjectGetMetalIRShaderStage(mtlIRObj);

	IRShaderReflection* reflection = IRShaderReflectionCreate();
	IRObjectGetReflection(mtlIRObj, IRShaderStageVertex, reflection);

	outShader.reflection.numResources = static_cast<unsigned int>(IRShaderReflectionGetResourceCount(reflection));

	IRMetalLibBinary* metalLib = IRMetalLibBinaryCreate();
	IRObjectGetMetalLibBinary(mtlIRObj, shaderStage, metalLib);
	const size_t metalLibSize = IRMetalLibGetBytecodeSize(metalLib);

	outShader.byteCode.resize(metalLibSize);
	IRMetalLibGetBytecode(metalLib, reinterpret_cast<uint8_t*>(outShader.byteCode.data()));

	IRCompilerDestroy(irCompiler);
	IRShaderReflectionDestroy(reflection);
	IRMetalLibBinaryDestroy(metalLib);
	IRObjectDestroy(dxilObj);
	IRObjectDestroy(mtlIRObj);

	return true;
}
#endif
}
