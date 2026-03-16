
#pragma once

#include <list>
#include <memory>

namespace kgx::core
{
class CommandThread;
class PooledResource;

template<class PooledType>
class ResourcePool final
{
public:
	ResourcePool(int initialPoolSize, std::function<std::unique_ptr<PooledType>()> creationFunction)
		: mCreationFunction(creationFunction)
	{
		for (int i = 0; i < initialPoolSize; ++i)
		{
			addNewResource();
		}
	}

	~ResourcePool() = default;

	ResourcePool(const ResourcePool&) noexcept            = delete;
	ResourcePool(ResourcePool&&) noexcept                 = delete;
	ResourcePool& operator=(const ResourcePool&) noexcept = delete;
	ResourcePool& operator=(ResourcePool&&) noexcept      = delete;

	[[nodiscard]]
	PooledType* getResource()
	{
		if (!mAvailable.empty())
		{
			PooledType* resource = mAvailable.back();
			mAvailable.pop_back();
			return resource;
		}

		return addNewResource();
	}

	void release(PooledType* resource)
	{
		mAvailable.push_back(resource);
	}

private:
	PooledType* addNewResource()
	{
		mAllResources.push_back(mCreationFunction());
		PooledType* resource = mAllResources.back().get();
		mAvailable.push_back(resource);

		return resource;
	}

	std::function<std::unique_ptr<PooledType>()> mCreationFunction;
	std::list<PooledType*> mAvailable;
	std::list<std::unique_ptr<PooledType>> mAllResources;
};
}
