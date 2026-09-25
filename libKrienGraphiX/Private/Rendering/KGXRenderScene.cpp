
#include "KGXRenderScene.h"

#include "KGXDrawPackage.h"
#include "Private/Core/RenderThread.h"
#include "Private/RHI/RenderHardwareInterface.h"

namespace
{
struct SceneConstantBufferData
{
	kgx::math::Matrix4X4 viewProjMatrix;
};
}

namespace kgx::rendering
{
void KGXRenderScene::addRenderObject(const std::shared_ptr<KGXMeshRenderObject>& renderObject)
{
	renderObject->setObjectId(static_cast<uint32_t>(mRenderObjects.size()));
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

	constexpr auto flags = static_cast<RHI::RHIResource::CreationFlags>(
		RHI::RHIResource::ShaderResource | RHI::RHIResource::ConstantBuffer);

	RHI::RHIBufferDescriptor cbDesc
	{
		.name = "SceneConstantBuffer",
		.bufferSize = sizeof(SceneConstantBufferData),
		.isBufferAligned = true,
		.isDynamic = true,
		.initialData = nullptr,
		.flags = flags
	};

	core::FrameCommandContext* frameContext = core::gRenderThread->getCurrentFrameContext();
	mSceneConstantBuffer = RHI::gPlatformRHI->createBuffer(frameContext->getRenderContext(), cbDesc);

	return mSceneConstantBuffer.get();
}

RHI::RHIBuffer* KGXRenderScene::updateAndGetSceneConstantBuffer()
{
	RHI::RHIBuffer* constantBuffer = getSceneConstantBuffer();

	SceneConstantBufferData uploadData{ getActiveCameraMatrix() };
	constantBuffer->copyBufferData(&uploadData, sizeof(SceneConstantBufferData), true);

	return constantBuffer;
}

RHI::RHIBuffer* KGXRenderScene::getMeshInstanceConstantBuffer()
{
	using namespace kgx;

	const size_t requiredBufferSize = std::max(mRenderObjects.size() * sizeof(MeshInstanceData), sizeof(MeshInstanceData));

	if (mMeshInstanceConstantBuffer && mMeshInstanceConstantBuffer->bufferSize() == requiredBufferSize)
	{
		return mMeshInstanceConstantBuffer.get();
	}

	if (mMeshInstanceConstantBuffer)
	{
		// Delete the old buffer after a couple of frames when it is not used anymore
		core::gRenderThread->enqueueCommand(core::DeferredRenderCommand(core::RenderThread::maxNumBufferedFrames,
			[oldBuffer = mMeshInstanceConstantBuffer.release()]()
			{
				delete oldBuffer;
			}));
	}

	constexpr auto flags = static_cast<RHI::RHIResource::CreationFlags>(
		RHI::RHIResource::ShaderResource | RHI::RHIResource::UnorderedAccess | RHI::RHIResource::ConstantBuffer);

	RHI::RHIBufferDescriptor cbDesc
	{
		.name = "MeshInstanceConstantBuffer",
		.bufferSize = requiredBufferSize,
		.isBufferAligned = true,
		.isDynamic = true,
		.initialData = nullptr,
		.flags = flags
	};

	core::FrameCommandContext* frameContext = core::gRenderThread->getCurrentFrameContext();
	mMeshInstanceConstantBuffer = RHI::gPlatformRHI->createBuffer(frameContext->getRenderContext(), cbDesc);

	return mMeshInstanceConstantBuffer.get();
}

RHI::RHIBuffer* KGXRenderScene::getObjectIdsBuffer()
{
	using namespace kgx;

	const size_t requiredBufferSize = std::max(mRenderObjects.size() * sizeof(uint32_t), sizeof(uint32_t));

	if (mObjectIdsBuffer && mObjectIdsBuffer->bufferSize() == requiredBufferSize)
	{
		return mObjectIdsBuffer.get();
	}

	if (mObjectIdsBuffer)
	{
		// Delete the old buffer after a couple of frames when it is not used anymore
		core::gRenderThread->enqueueCommand(core::DeferredRenderCommand(core::RenderThread::maxNumBufferedFrames,
			[oldBuffer = mObjectIdsBuffer.release()]()
			{
				delete oldBuffer;
			}));
	}

	constexpr RHI::RHIResource::CreationFlags flags = RHI::RHIResource::VertexBuffer;

	std::vector<uint32_t> objectIds;
	objectIds.resize(mRenderObjects.size());
	for (size_t i = 0; i < mRenderObjects.size(); ++i)
	{
		objectIds[i] = mRenderObjects[i]->getObjectId();
	}

	RHI::RHIBufferDescriptor cbDesc
	{
		.name = "ObjectIdsBuffer",
		.bufferSize = requiredBufferSize,
		.isBufferAligned = true,
		.isDynamic = true,
		.initialData = objectIds.data(),
		.flags = flags
	};

	core::FrameCommandContext* frameContext = core::gRenderThread->getCurrentFrameContext();
	mObjectIdsBuffer = RHI::gPlatformRHI->createBuffer(frameContext->getRenderContext(), cbDesc);

	return mObjectIdsBuffer.get();
}
}
