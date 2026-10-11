#include "MTLDepthStencilManager.h"

#include <ranges>
#include <xxh3.h>

#include "MTLRenderHardwareInterface.h"

namespace
{
XXH64_hash_t hash(const kgx::rendering::DepthStencilBinding& depthStencilBinding)
{
	XXH3_state_t* state = XXH3_createState();

	if (!state)
	{
		KGXLOG_CRITICAL("Error creating XXH3 state. Aborting.");
		abort();
	}

	XXH3_64bits_reset(state);

	XXH3_64bits_update(state, &depthStencilBinding.depthWriteEnable, sizeof(bool));
	XXH3_64bits_update(state, &depthStencilBinding.depthFunction, sizeof(kgx::rendering::DepthComparisonFunction));
	XXH3_64bits_update(state, &depthStencilBinding.stencilEnable, sizeof(bool));
	XXH3_64bits_update(state, &depthStencilBinding.stencilReadMask, sizeof(uint8_t));
	XXH3_64bits_update(state, &depthStencilBinding.stencilWriteMask, sizeof(uint8_t));

	XXH64_hash_t result = XXH3_64bits_digest(state);
	XXH3_freeState(state);
	return result;
}

MTL::CompareFunction toMTLComparisonFunction(kgx::rendering::DepthComparisonFunction comparisonFunction)
{
	using namespace kgx::rendering;

	switch (comparisonFunction)
	{
	case DepthComparisonFunction::Less:
		return MTL::CompareFunctionLess;
	case DepthComparisonFunction::Equal:
		return MTL::CompareFunctionEqual;
	case DepthComparisonFunction::LessEqual:
		return MTL::CompareFunctionLessEqual;
	case DepthComparisonFunction::Greater:
		return MTL::CompareFunctionGreater;
	case DepthComparisonFunction::NotEqual:
		return MTL::CompareFunctionNotEqual;
	case DepthComparisonFunction::GreaterEqual:
		return MTL::CompareFunctionGreaterEqual;
	case DepthComparisonFunction::Always:
		return MTL::CompareFunctionAlways;
	case DepthComparisonFunction::Never:
	default:
		return MTL::CompareFunctionNever;
	}
}
}

namespace kgx::RHI
{
MTLDepthStencilManager::MTLDepthStencilManager()
{
	rendering::DepthStencilBinding depthWriteLess =
	{
		.depthWriteEnable = true,
		.depthFunction = rendering::DepthComparisonFunction::Less,
	};
	addDepthStencilState(depthWriteLess);

	rendering::DepthStencilBinding depthWriteGreater =
	{
		.depthWriteEnable = true,
		.depthFunction = rendering::DepthComparisonFunction::Greater,
	};
	addDepthStencilState(depthWriteGreater);
}

MTL::DepthStencilState* MTLDepthStencilManager::getMTLDepthStencilState(
	const rendering::DepthStencilBinding& depthStencilBinding)
{
	XXH64_hash_t bindingHash = ::hash(depthStencilBinding);
	if (auto dsState = mDepthStencilStateCache.find(bindingHash); dsState != mDepthStencilStateCache.end())
	{
		return dsState->second.get();
	}

	KGXLOG_WARN_ONCE("DepthStencilState not found. Creating new one.")
	return addDepthStencilState(depthStencilBinding);
}

MTL::DepthStencilState* MTLDepthStencilManager::addDepthStencilState(const rendering::DepthStencilBinding& depthStencilBinding)
{
	XXH64_hash_t bindingHash = ::hash(depthStencilBinding);

	MTL::DepthStencilDescriptor* dsDesc = MTL::DepthStencilDescriptor::alloc()->init();
	dsDesc->setDepthCompareFunction(toMTLComparisonFunction(depthStencilBinding.depthFunction));
	dsDesc->setDepthWriteEnabled(depthStencilBinding.depthWriteEnable);

	//TODO(KL): Implement stencildescriptor
	//MTL::StencilDescriptor* stencilDesc = MTL::StencilDescriptor::alloc()->init();
	//dsDesc->setFrontFaceStencil()

	auto mtlDevice = getMTLRHI()->getMTLDevice();
	MTL::DepthStencilState* depthStencilState = mtlDevice->getNativeDevice()->newDepthStencilState(dsDesc);
	//stencilDesc->release();
	dsDesc->release();

	//TODO(KL): Create a label for the depthstencilstate

	mDepthStencilStateCache[bindingHash] = NS::TransferPtr(depthStencilState);

	return depthStencilState;
}
}
