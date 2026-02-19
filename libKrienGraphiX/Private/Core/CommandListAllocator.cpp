
#include "CommandListAllocator.h"

#include <cassert>

#include "Private/RHI/RenderHardwareInterface.h"

namespace kgx::core
{
RHI::RHIGraphicsCommandListHandle CommandListAllocator::createGraphicsCommandList(RHI::RHIGraphicsPipelineState* initialState)
{
	//TODO(KL): Enforce this is only called on the RenderThread

	// For now I just loop through the entire list to get the next one that is open. If this causes too much of a performance hit I will think of a different way to handle this
	for (auto& [_, commandListAllocation] : mCommandLists)
	{
		if (commandListAllocation.isFree)
		{
			commandListAllocation.isFree = false;
			return RHI::RHIGraphicsCommandListHandle(*commandListAllocation.commandList);
		}
	}

	const std::shared_ptr<RHI::RHIGraphicsCommandList> newCommandList = RHI::PlatformRHI->createGraphicsCommandList(this, initialState);
	CommandListAllocation newAllocation
	{
		.isFree = false,
		.commandList = newCommandList
	};

	mCommandLists.insert(std::make_pair(newCommandList.get(), std::move(newAllocation)));

	return RHI::RHIGraphicsCommandListHandle(*newCommandList);
}

void CommandListAllocator::releaseGraphicsCommandList(RHI::RHIGraphicsCommandList* commandList)
{
	const auto it = mCommandLists.find(commandList);
	assert(it != mCommandLists.cend());

	if (it != mCommandLists.end())
	{
		it->second.isFree = true;
	}
}
}
