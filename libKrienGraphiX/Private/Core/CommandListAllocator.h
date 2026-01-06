
#pragma once

#include <memory>
#include <unordered_map>

#include "Private/RHI/RHIGraphicsCommandList.h"

namespace kgx::core
{
class CommandThread;

class CommandListAllocator final
{
public:
	CommandListAllocator() = default;
	~CommandListAllocator() = default;

	CommandListAllocator(const CommandListAllocator&) noexcept            = delete;
	CommandListAllocator(CommandListAllocator&&) noexcept                 = delete;
	CommandListAllocator& operator=(const CommandListAllocator&) noexcept = delete;
	CommandListAllocator& operator=(CommandListAllocator&&) noexcept      = delete;

	[[nodiscard]] RHI::RHIGraphicsCommandListHandle createGraphicsCommandList(RHI::RHIGraphicsPipelineState* initialState);
	void releaseGraphicsCommandList(RHI::RHIGraphicsCommandList* commandList);

private:
	struct CommandListAllocation
	{
		bool isFree = false;
		std::shared_ptr<RHI::RHIGraphicsCommandList> commandList;
	};

	std::unordered_map<RHI::RHIGraphicsCommandList*, CommandListAllocation> mCommandLists;
};
}
