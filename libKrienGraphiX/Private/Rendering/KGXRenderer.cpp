
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

	assert(vertexShader);
	assert(pixelShader);

	std::vector<VertexInputElement> layoutDesc = {VertexPositionInput, VertexColorInput, VertexObjectIDInput};
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
KGXRenderer::KGXRenderer(
	const core::KGXViewport& Viewport,
	const RHI::RHITextureHandle& OutputRenderTarget,
	const RHI::RHITextureHandle& DepthStencil)
	: mViewport(Viewport), mOutputRTV(OutputRenderTarget), mDSV(DepthStencil)
{
}

void KGXRenderer::RenderFrame()
{
	KGXLOG_TRACE("Starting RenderFrame");

	auto frameContext = core::gRenderThread->getCurrentFrameContext();

	//TODO(KL): Process meshes: view culling

	KGXRenderCommandContext renderContext(*frameContext);

	auto* rhiPlatform = core::gRenderThread->getRHIPlatformPtr();
	rhiPlatform->beginFrame(frameContext->getRenderContext(), mOutputRTV);

	// Main pass
	{
		static std::array<float, 4> lightSteelBlue = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
		TextureBinding outTargetBinding
		{
			.texture = mOutputRTV,
			.loadAction = TextureLoadAction::Clear,
			.storeAction = TextureStoreAction::Store,
			.clearValue = { lightSteelBlue }
		};

		KGXRenderPassParameters mainPassParameters
		{
			.name = "mainPass",
			.viewport = mViewport,
			.depthStencilView = mDSV,
			.inputTextures = {},
			.outputTextures = {outTargetBinding},
			.pso = getStaticPSO()
		};

		renderContext.addRenderPass(mainPassParameters);
	}

	renderContext.runPasses();

	rhiPlatform->endFrame(frameContext->getRenderContext(), mOutputRTV);
	frameContext->endFrame();

	KGXLOG_TRACE("End RenderFrame");
}
}
