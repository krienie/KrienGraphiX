
#include "KGXRenderScene.h"

#include "KGXDrawPackage.h"
#include "Private/Core/RenderThread.h"
#include "Private/RHI/RenderHardwareInterface.h"
#include "Private/RHI/RHIGraphicsCommandList.h"

namespace
{
__declspec(align(256u)) struct SceneConstantBufferData
{
	kgx::math::Matrix4X4 viewProjMatrix;
};
}

namespace kgx::rendering
{
void KGXRenderScene::addRenderObject(const std::shared_ptr<KGXMeshRenderObject>& renderObject)
{
	mRenderObjects.push_back(renderObject);

	renderObject->createRenderResources();
}

void KGXRenderScene::updateRenderObjectTransforms(const std::vector<MeshTransformUpdateParams>& transformUpdates)
{
	for (const MeshTransformUpdateParams& update : transformUpdates)
	{
		update.meshToUpdate->updateTransform(update.transform);
	}
}

void KGXRenderScene::updateActiveCameraMatrix(const math::Matrix4X4& viewProjection)
{
	mActiveCameraMatrix = viewProjection;
}

const math::Matrix4X4& KGXRenderScene::getActiveCameraMatrix() const
{
	return mActiveCameraMatrix;
}

RHI::RHIBuffer* KGXRenderScene::getSceneConstantBuffer()
{
	using namespace kgx;

	if (mSceneConstantBuffer)
	{
		return mSceneConstantBuffer.get();
	}

	RHI::RHIGraphicsCommandList* commandList = core::gRenderThread->getCurrentFrameCommandList();

	constexpr auto flags = static_cast<RHI::RHIResource::CreationFlags>(
		RHI::RHIResource::ShaderResource | RHI::RHIResource::ConstantBuffer);

	RHI::RHIBufferDescriptor cbDesc
	{
		.name = "SceneConstantBuffer",
		.bufferSize = sizeof(SceneConstantBufferData),
		.bufferRegister = 0,
		.isBufferAligned = true,
		.isDynamic = true,
		.initialData = nullptr,
		.flags = flags
	};

	mSceneConstantBuffer = RHI::gPlatformRHI->createBuffer(commandList, cbDesc);

	return mSceneConstantBuffer.get();
}

RHI::RHIBuffer* KGXRenderScene::updateAndGetSceneConstantBuffer()
{
	RHI::RHIBuffer* constantBuffer = getSceneConstantBuffer();

	SceneConstantBufferData uploadData{ getActiveCameraMatrix() };
	constantBuffer->copyBufferData(&uploadData, sizeof(SceneConstantBufferData), true);

	return constantBuffer;
}
}
