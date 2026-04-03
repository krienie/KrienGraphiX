
#include "ShaderCompiler/ShaderCompiler.h"

#ifdef _WIN32
#include <wrl/client.h>
#endif

#include <dxc/dxcapi.h>

#include <filesystem>
#include <iostream>
#include <sstream>

#ifdef _WIN32
using namespace Microsoft::WRL;
#elif __APPLE__
#include "MacOS/DXILToMetalIRConverter.h"

template <class T>
using ComPtr = CComPtr<T>;
#endif

namespace
{
ComPtr<IDxcUtils> utils = nullptr;
ComPtr<IDxcCompiler3> compiler = nullptr;
}

namespace kgx
{
bool ShaderCompiler::compileShader(const std::string& sourceFile, const std::string& mainEntry, const std::string& target, bool includeDebugInfo, CompiledShader& outCompiledShader)
{
	if (!std::filesystem::exists(sourceFile))
	{
		outCompiledShader = CompiledShader();
		return false;
	}

	if (!utils || !compiler)
	{
		// Create compiler and utils.
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	}

	// Create default include handler
	ComPtr<IDxcIncludeHandler> includeHandler;
	utils->CreateDefaultIncludeHandler(&includeHandler);

	const std::filesystem::path fileName = std::filesystem::path(sourceFile).filename();

	// COMMAND LINE: dxc myshader.hlsl -E main -T ps_6_0 -Zi -D MYDEFINE=1 -Fo myshader.bin -Fd myshader.pdb -Qstrip_reflect
	const std::wstring wSourceFile(sourceFile.cbegin(), sourceFile.cend());
	const std::wstring wMainEntry(mainEntry.cbegin(), mainEntry.cend());
	const std::wstring wTarget(target.cbegin(), target.cend());

	std::vector<LPCWSTR> compileArgs =
	{
		wSourceFile.c_str(),
		L"-E", wMainEntry.c_str(),
		L"-T", wTarget.c_str(),
		L"-Qstrip_reflect"
	};

	const std::wstring stem = fileName.stem().wstring();

	// Set output file
	compileArgs.push_back(L"-Fo");

	const std::wstring binPath = stem + L".bin";
	compileArgs.push_back(binPath.c_str());

	std::wstring pdbPath = stem + L".pdb";

	if (includeDebugInfo)
	{
		compileArgs.push_back(L"-Zi");
		compileArgs.push_back(L"-Qembed_debug");

		compileArgs.push_back(L"-Fd");
		compileArgs.push_back(pdbPath.c_str());

		compileArgs.push_back(L"-O0");
	}

	ComPtr<IDxcBlobEncoding> sourceBlob = nullptr;
	HRESULT res = utils->LoadFile(wSourceFile.c_str(), nullptr, &sourceBlob);
	if (FAILED(res))
	{
		std::cout << "ShaderCompiler::compileShader: failed to load file " << sourceFile << "\n";
		outCompiledShader = CompiledShader();
		return false;
	}

	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
	sourceBuffer.Size = sourceBlob->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.

	// Compile it with specified arguments.
	ComPtr<IDxcResult> results;
	compiler->Compile(
		&sourceBuffer,
		compileArgs.data(),
		static_cast<UINT32>(compileArgs.size()),
#ifdef _WIN32
		includeHandler.Get(),
#elif __APPLE__
		includeHandler,
#endif
		IID_PPV_ARGS(&results)
	);

	ComPtr<IDxcBlobUtf8> errors = nullptr;
	results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
	// IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
	if (errors != nullptr && errors->GetStringLength() != 0)
	{
		std::wcout << L"Warnings and Errors: " << errors->GetStringPointer() << "\n";
	}

	HRESULT hrStatus;
	results->GetStatus(&hrStatus);
	if (FAILED(hrStatus))
	{
		std::wcout << L"Compilation Failed\n";
		outCompiledShader = CompiledShader();
		return false;
	}


	CompiledShader shader;

	// Save shader binary.
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	ComPtr<IDxcBlobWide> shaderName = nullptr;
	results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &shaderName);
	if (shaderBlob != nullptr)
	{
		shader.byteCode.resize(shaderBlob->GetBufferSize());
		std::memcpy(shader.byteCode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
	}

	if (includeDebugInfo)
	{
		ComPtr<IDxcBlob> pdb = nullptr;
		ComPtr<IDxcBlobWide> pdbName = nullptr;
		results->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdbName);
		{
			shader.pdb.resize(pdb->GetBufferSize());
			std::memcpy(shader.pdb.data(), pdb->GetBufferPointer(), pdb->GetBufferSize());
		}
	}

#if __APPLE__
	if (!DXILToMetalIRConverter::convertToMetalIR(shader.byteCode, mainEntry, shader))
	{
		std::wcout << L"Conversion from DXIL to Metal IR failed.\n";
		outCompiledShader = CompiledShader();
		return false;
	}
#endif

	ComPtr<IDxcBlob> shaderHash = nullptr;
	results->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&shaderHash), nullptr);
	if (shaderHash != nullptr)
	{
		std::wcout << L"Shader hash: ";
		auto* hashBuf = static_cast<DxcShaderHash*>(shaderHash->GetBufferPointer());
		for (unsigned char i : hashBuf->HashDigest)
		{
			std::wcout << std::hex << i;
		}
		std::wcout << std::endl;
	}

	shader.reflection.mainEntry = mainEntry;

	outCompiledShader = std::move(shader);

	return true;
}
}
