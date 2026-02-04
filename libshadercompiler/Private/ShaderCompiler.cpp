
#include "ShaderCompiler/ShaderCompiler.h"

#include <dxc/d3d12shader.h>
#include <dxc/dxcapi.h>
#include <wrl/client.h>

#include <filesystem>
#include <iostream>
#include <sstream>


using namespace Microsoft::WRL;

namespace
{
ComPtr<IDxcUtils> mUtils = nullptr;
ComPtr<IDxcCompiler3> mCompiler = nullptr;
}

namespace kgx
{
bool ShaderCompiler::compileShader(const std::string& sourceFile, const std::string& mainEntry, const std::string& target, bool includeDebugInfo, CompiledShader& OutCompiledShader)
{
	if (!std::filesystem::exists(sourceFile))
	{
		OutCompiledShader = CompiledShader();
		return false;
	}

	if (!mUtils || !mCompiler)
	{
		// Create compiler and utils.
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mUtils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mCompiler));
	}

	// Create default include handler
	ComPtr<IDxcIncludeHandler> pIncludeHandler;
	mUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

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

	if (includeDebugInfo)
	{
		const std::wstring stem = fileName.stem().wstring();

		compileArgs.push_back(L"-Zi");
		compileArgs.push_back(L"-Fo");

		std::wstringstream binSs;
		binSs << stem << L".bin";

		compileArgs.push_back(binSs.str().c_str());
		compileArgs.push_back(L"-Fd");

		std::wstringstream pdbSs;
		pdbSs << stem << L".pdb";

		compileArgs.push_back(pdbSs.str().c_str());
	}
	
	// Open source file.
	ComPtr<IDxcBlobEncoding> pSource = nullptr;
	HRESULT res = mUtils->LoadFile(wSourceFile.c_str(), nullptr, &pSource);
	if (FAILED(res))
	{
		std::cout << "ShaderCompiler::compileShader: failed to load file " << sourceFile << "\n";
		OutCompiledShader = CompiledShader();
		return false;
	}

	DxcBuffer Source;
	Source.Ptr = pSource->GetBufferPointer();
	Source.Size = pSource->GetBufferSize();
	Source.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.


	// Compile it with specified arguments.
	ComPtr<IDxcResult> pResults;
	mCompiler->Compile(
		&Source,                                  // Source buffer.
		compileArgs.data(),                       // Array of pointers to arguments.
		static_cast<UINT32>(compileArgs.size()),  // Number of arguments.
		pIncludeHandler.Get(),                    // User-provided interface to handle #include directives (optional).
		IID_PPV_ARGS(&pResults)                   // Compiler output status, buffer, and errors.
	);

	// Print errors if present.
	ComPtr<IDxcBlobUtf8> pErrors = nullptr;
	pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
	// Note that d3dcompiler would return null if no errors or warnings are present.  
	// IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
	if (pErrors != nullptr && pErrors->GetStringLength() != 0)
	{
		std::wcout << L"Warnings and Errors: " << pErrors->GetStringPointer() << "\n";
	}

	// Quit if the compilation failed.
	HRESULT hrStatus;
	pResults->GetStatus(&hrStatus);
	if (FAILED(hrStatus))
	{
		std::wcout << L"Compilation Failed\n";
		OutCompiledShader = CompiledShader();
		return false;
	}


	CompiledShader shader;

	// Save shader binary.
	ComPtr<IDxcBlob> pShader = nullptr;
	ComPtr<IDxcBlobUtf16> pShaderName = nullptr;
	pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pShaderName);
	if (pShader != nullptr)
	{
		shader.byteCode.resize(pShader->GetBufferSize());
		std::memcpy(shader.byteCode.data(), pShader->GetBufferPointer(), pShader->GetBufferSize());
	}

	// Save pdb.
	ComPtr<IDxcBlob> pPDB = nullptr;
	ComPtr<IDxcBlobUtf16> pPDBName = nullptr;
	pResults->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pPDB), &pPDBName);
	{
		// Note that if you don't specify -Fd, a pdb name will be automatically generated. Use this file name to save the pdb so that PIX can find it quickly.
		shader.pdb.resize(pPDB->GetBufferSize());
		std::memcpy(shader.pdb.data(), pPDB->GetBufferPointer(), pPDB->GetBufferSize());
	}

	// Print hash.
	ComPtr<IDxcBlob> pHash = nullptr;
	pResults->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&pHash), nullptr);
	if (pHash != nullptr)
	{
		std::wcout << L"Shader hash: ";
		auto * pHashBuf = static_cast<DxcShaderHash*>(pHash->GetBufferPointer());
		for (unsigned char i : pHashBuf->HashDigest)
		{
			std::wcout << std::hex << i;
		}
		std::wcout << std::endl;
	}


	// Get separate reflection.
	ComPtr<IDxcBlob> pReflectionData;
	pResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr);
	if (pReflectionData != nullptr)
	{
		// Optionally, save reflection blob for later here.

		// Create reflection interface.
		DxcBuffer ReflectionData;
		ReflectionData.Encoding = DXC_CP_ACP;
		ReflectionData.Ptr = pReflectionData->GetBufferPointer();
		ReflectionData.Size = pReflectionData->GetBufferSize();

		ComPtr<ID3D12ShaderReflection> pReflection;
		mUtils->CreateReflection(&ReflectionData, IID_PPV_ARGS(&pReflection));

		// Use reflection interface here.
		D3D12_SHADER_DESC shaderDesc;
		res = pReflection->GetDesc(&shaderDesc);
		if (FAILED(res))
		{
			std::wcout << L"Shader reflection failed.\n";
			OutCompiledShader = std::move(shader);
			return false;
		}
	}

	OutCompiledShader = std::move(shader);

	return true;
}
}
