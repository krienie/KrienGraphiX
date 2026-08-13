
#pragma once

#include <vector>
#include <xxh3.h>

#include "Private/Core/PrivateDefinitions.h"
#include "Private/RHI/RHIGraphicsPipelineState.h"
#include "Private/RHI/RHIResourceView.h"

namespace kgx::rendering
{
struct KGXConstantBufferUpdatePackage
{
	RHI::RHIBuffer* buffer;
	std::vector<uint8_t> uploadData;

	template<typename DataStruct>
	void setData(const DataStruct& dataStruct)
	{
		uploadData.resize(sizeof(DataStruct));
		memcpy(uploadData.data(), &dataStruct, sizeof(DataStruct));
	}
};

struct KGXDrawStatePackage
{
	//TODO(KL): Use proper handles instead of raw pointers
	core::KGXViewport viewport;
	RHI::RHIGraphicsPipelineState* pso;
	std::vector<RHI::RHIResourceView*> renderTargetViews;
	RHI::RHIResourceView* depthStencilView;
};
}

namespace kgx
{
XXH64_hash_t hash(const rendering::KGXDrawStatePackage& drawStatePackage);
}
