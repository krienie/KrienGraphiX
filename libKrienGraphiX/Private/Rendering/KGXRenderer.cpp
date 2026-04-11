
#include "KGXRenderer.h"

#include <filesystem>

#include "KrienGraphiX/Core/Logging.h"
#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/RHI/RHIDescriptors.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"

#ifdef __APPLE__
#include <filesystem>
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace
{
__declspec(align(256u)) struct ConstantBufferData
{
	kgx::math::Matrix4X4 modelMatrix;
	kgx::math::Matrix4X4 viewProjMatrix;
};

std::unique_ptr<kgx::RHI::RHIGraphicsPipelineState> staticPSO;
std::unique_ptr<kgx::RHI::RHIBuffer> staticConstantBuffer;

//TODO(KL): Move this to a separate file/class. Something like a resource manager
#ifdef __APPLE__
std::string getFullPath(const std::string& relativePathStr)
{
	namespace fs = std::filesystem;

	const fs::path relativePath = relativePathStr;

	CFBundleRef mainBundle = CFBundleGetMainBundle();

	const CFStringRef resName = CFStringCreateWithCString(nullptr, relativePath.stem().c_str(), kCFStringEncodingUTF8);
	const CFStringRef subDirName = CFStringCreateWithCString(nullptr, relativePath.parent_path().c_str(), kCFStringEncodingUTF8);
	const CFStringRef resExt  = CFStringCreateWithCString(nullptr, relativePath.extension().c_str(), kCFStringEncodingUTF8);

	const CFURLRef fileURL = CFBundleCopyResourceURL(mainBundle, resName, resExt, subDirName);

	CFRelease(resName);
	CFRelease(subDirName);
	CFRelease(resExt);

	if (!fileURL)
	{
		return "";
	}

	// 3. Convert URL to a POSIX path
	char path[PATH_MAX];
	if (CFURLGetFileSystemRepresentation(fileURL, true, reinterpret_cast<UInt8*>(path), PATH_MAX))
	{
		CFRelease(fileURL);
		return {path};
	}

	CFRelease(fileURL);
	return "";
}
#endif

kgx::RHI::RHIGraphicsPipelineState* getStaticPSO()
{
	using namespace kgx;

	if (staticPSO)
	{
		return staticPSO.get();
	}

	rendering::KGXShaderCache* shaderCache = core::gRenderThread->getShaderCachePtr();

#ifdef __APPLE__
	const auto vertexShaderPath = getFullPath("Shaders/DefaultVS.hlsl");
	const auto pixelShaderPath = getFullPath("Shaders/DefaultPS.hlsl");
#else
	const auto vertexShaderPath = std::filesystem::absolute("./Shaders/DefaultVS.hlsl").string();
	const auto pixelShaderPath = std::filesystem::absolute("./Shaders/DefaultPS.hlsl").string();
#endif

	auto* vertexShader = shaderCache->loadShaderFromFile(vertexShaderPath, "main", RHI::RHIShader::ShaderType::Vertex);
	auto* pixelShader = shaderCache->loadShaderFromFile(pixelShaderPath, "main", RHI::RHIShader::ShaderType::Pixel);

	std::vector<VertexInputElement> layoutDesc = {VertexPositionInput, VertexColorInput};
	vertexShader->setVertexInputLayout(layoutDesc);

	RHI::RHIGraphicsPipelineStateDescriptor psoDesc =
	{
		.vs = vertexShader,
		.ps = pixelShader,
		.primitiveTopology = RHI::Triangle,
		.numRenderTargets = 1,
		.depthStencilFormat = RHI::RHIPixelFormat::D24_unorm_S8_uint
	};

	//TODO(KL): Get these pixelformats from the buffers themselves
	psoDesc.renderTargetFormats[0] = RHI::RHIPixelFormat::R10G10B10A2_unorm;

	staticPSO = RHI::gPlatformRHI->createGraphicsPipelineState(psoDesc);

	return staticPSO.get();
}

kgx::RHI::RHIBuffer* getStaticConstantBuffer()
{
	using namespace kgx;

	if (staticConstantBuffer)
	{
		return staticConstantBuffer.get();
	}

	RHI::RHIGraphicsCommandList* commandList = core::gRenderThread->getCurrentFrameCommandList();

	constexpr auto flags = static_cast<RHI::RHIResource::CreationFlags>(
		RHI::RHIResource::ShaderResource | RHI::RHIResource::ConstantBuffer);

	RHI::RHIBufferDescriptor cbDesc
	{
		.name = "StaticConstantBuffer",
		.bufferSize = sizeof(ConstantBufferData),
		.bufferRegister = 0,
		.isBufferAligned = true,
		.isDynamic = true,
		.initialData = nullptr,
		.flags = flags
	};

	staticConstantBuffer = RHI::gPlatformRHI->createBuffer(commandList, cbDesc);

	return staticConstantBuffer.get();
}
}

namespace kgx::rendering
{
KGXRenderer::KGXRenderer(const core::KGXViewport& Viewport, RHI::RHIResourceView& OutputRenderTarget, RHI::RHIResourceView& DepthStencil)
	: mViewport(Viewport), mOutputRTV(&OutputRenderTarget), mDSV(&DepthStencil)
{
}

void KGXRenderer::RenderFrame()
{
	KGXLOG_TRACE("Starting RenderFrame");

	auto frameContext = core::gRenderThread->getCurrentFrameContext();
	auto commandList = frameContext->getCommandList();

	auto* OutputRenderTarget = static_cast<RHI::RHITexture2D*>(mOutputRTV->getViewedResource());

	RHI::gPlatformRHI->beginFrame(commandList, OutputRenderTarget);

	commandList->setViewport(mViewport);

	static float lightSteelBlue[4] = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

	commandList->clearRenderTargetView(mOutputRTV, lightSteelBlue);
#ifndef __APPLE__
	commandList->clearDepthStencilView(mDSV, RHI::DepthStencilFlags::DepthStencilClear, 1.0f, 0);
#endif

	commandList->setRenderTargets({ mOutputRTV }, mDSV);

	RHI::RHIGraphicsPipelineState* staticPSO = getStaticPSO();
	commandList->setPipelineState(staticPSO);

	const RHI::RHIBuffer* staticConstantBuff = getStaticConstantBuffer();
	commandList->setConstantBuffer(staticConstantBuff);

	auto* renderScene = core::RenderCore::get()->getScenePtr()->getRenderScenePtr();

	ConstantBufferData cbData;
	cbData.viewProjMatrix = renderScene->getActiveCameraMatrix();

	//TODO(KL): record mesh draw commands. Simple for now
	for (auto& renderObject : *renderScene)
	{
		//TODO(KL): Create a small abstraction for using RootConstants/PushConstants instead of filling a CB for these small values.
		cbData.modelMatrix = renderObject->getTransform();
		memcpy(staticConstantBuff->mappedDataPtr(), &cbData, sizeof(ConstantBufferData));

		commandList->drawMeshRenderObject(renderObject.get());
	}

	RHI::gPlatformRHI->endFrame(commandList, OutputRenderTarget);
	frameContext->endFrame();

	KGXLOG_TRACE("End RenderFrame");
}
}
