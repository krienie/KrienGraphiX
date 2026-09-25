
#pragma once

#include "Private/Core/ResourcePool.h"

namespace kgx::rendering
{
struct KGXRenderPassParameters;
class KGXMeshRenderObject;
}

namespace kgx::RHI
{
class RHIBuffer;
class RHIPlatform;

class RHIRenderContext : public core::PooledType<RHIRenderContext>
{
public:
	RHIRenderContext(core::ResourcePool<RHIRenderContext>& parentPool);
	~RHIRenderContext() override = default;

	virtual void setConstantBuffers(
		const RHIBuffer* sceneConstantBuffer,
		const RHIBuffer* meshInstanceConstantBuffer,
		const RHIBuffer* objectIdsBuffer) = 0;

	virtual void activateRenderPass(const rendering::KGXRenderPassParameters& renderPassParameters) = 0;
	virtual void drawMeshRenderObject(const rendering::KGXMeshRenderObject* renderObject) = 0;

	virtual void close() = 0;
	virtual void execute(bool waitForCompletion) = 0;
	virtual void reset() = 0;
};
}
