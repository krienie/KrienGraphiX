
#include "KGXRenderer.h"

#include <filesystem>

#include "Private/Core/RenderCore.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/RHI/RHIDescriptors.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"

namespace
{
__declspec(align(256u)) struct ConstantBufferData
{
	kgx::math::Matrix4X4 modelMatrix;
	kgx::math::Matrix4X4 viewProjMatrix;
};

std::unique_ptr<kgx::RHI::RHIGraphicsPipelineState> staticPSO;
std::unique_ptr<kgx::RHI::RHIBuffer> staticConstantBuffer;

kgx::RHI::RHIGraphicsPipelineState* getStaticPSO()
{
	using namespace kgx;

	if (staticPSO)
	{
		return staticPSO.get();
	}

	const auto* renderThread = core::RenderCore::get()->getRenderThreadPtr();
	rendering::KGXShaderCache* shaderCache = renderThread->getShaderCache();

	const auto vertexShaderPath = std::filesystem::absolute("./Shaders/DefaultVS.hlsl");
	const auto pixelShaderPath = std::filesystem::absolute("./Shaders/DefaultPS.hlsl");

	auto* vertexShader = shaderCache->loadShaderFromFile(vertexShaderPath.string(), "main", RHI::RHIShader::ShaderType::Vertex);
	auto* pixelShader = shaderCache->loadShaderFromFile(pixelShaderPath.string(), "main", RHI::RHIShader::ShaderType::Pixel);

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

	staticPSO = RHI::PlatformRHI->createGraphicsPipelineState(psoDesc);

	return staticPSO.get();
}

kgx::RHI::RHIBuffer* getStaticConstantBuffer()
{
	using namespace kgx;

	if (staticConstantBuffer)
	{
		return staticConstantBuffer.get();
	}

	const core::RenderThread* renderThreadPtr = core::RenderCore::get()->getRenderThreadPtr();
	RHI::RHIGraphicsCommandListHandle commandList = renderThreadPtr->getCommandList();

	constexpr RHI::RHIResource::CreationFlags flags = static_cast<RHI::RHIResource::CreationFlags>(RHI::RHIResource::ShaderResource | RHI::RHIResource::ConstantBuffer);

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

	staticConstantBuffer = RHI::PlatformRHI->createBuffer(commandList.get(), cbDesc);

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
	const auto* renderThread = core::RenderCore::get()->getRenderThreadPtr();
	RHI::RHIGraphicsCommandListHandle commandList = renderThread->getCommandList();

	auto* OutputRenderTarget = static_cast<RHI::RHITexture2D*>(mOutputRTV->getViewedResource());

	RHI::PlatformRHI->beginFrame(commandList.get(), OutputRenderTarget);
	
	commandList->setViewport(mViewport);
	
	static float lightSteelBlue[4] = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

	commandList->clearRenderTargetView(mOutputRTV, lightSteelBlue);
	commandList->clearDepthStencilView(mDSV, RHI::DepthStencilFlags::DepthStencilClear, 1.0f, 0);

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
		cbData.modelMatrix = renderObject->getTransform();
		memcpy(staticConstantBuff->mappedDataPtr(), &cbData, sizeof(ConstantBufferData));

		commandList->drawMeshRenderObject(renderObject.get());
	}

	RHI::PlatformRHI->endFrame(commandList.get(), OutputRenderTarget);
}
}
