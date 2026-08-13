
#include "KGXRenderer.h"

#include <array>
#include <filesystem>

#include "KGXDrawPackage.h"
#include "KGXRenderCommandContext.h"
#include "KGXRenderPass.h"
#include "KrienGraphiX/Core/Logging.h"
#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/RHI/RHIDefinitions.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"

#ifdef __APPLE__
#include <filesystem>
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace
{
std::unique_ptr<kgx::RHI::RHIGraphicsPipelineState> staticPSO;

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

	KGXRenderCommandContext renderContext(*frameContext);
	TextureHandle outTargetHandle = renderContext.registerTexture(mOutputRTV);

	RHI::gPlatformRHI->beginFrame(commandList, OutputRenderTarget);

	auto* mainPass = renderContext.addNewRenderPass("mainPass");

	static std::array<float, 4> lightSteelBlue = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
	TextureBinding outTargetBinding
	{
		.texture = outTargetHandle,
		.loadAction = TextureLoadAction::Clear,
		.storeAction = TextureStoreAction::Store,
		.clearColor = lightSteelBlue
	};

	mainPass->initPass({}, {outTargetBinding}, mViewport, mDSV);

	auto* renderScene = core::RenderCore::get()->getScenePtr()->getRenderScenePtr();

	RHI::RHIGraphicsPipelineState* staticPSO = getStaticPSO();

	//TODO(KL): Not used right now
	KGXConstantBufferUpdatePackage dummy;

	for (auto& renderObject : *renderScene)
	{
		mainPass->drawMesh(staticPSO, dummy, renderObject.get());
	}

	renderContext.runCommands();

	RHI::gPlatformRHI->endFrame(commandList, OutputRenderTarget);
	frameContext->endFrame();

	KGXLOG_TRACE("End RenderFrame");
}
}
