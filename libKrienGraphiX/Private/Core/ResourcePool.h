
#pragma once

#include <list>
#include <memory>

namespace kgx::core
{
template<class PooledType>
class ResourcePool;

template<class ResourceType>
class PooledType
{
public:
	virtual ~PooledType() = default;

	PooledType(ResourcePool<ResourceType>& parentPool)
		: mParentPool(parentPool) {}

	void release()
	{
		releaseImpl();
		mParentPool.returnResource(this);
	}

protected:
	ResourcePool<ResourceType>& mParentPool;

private:
	virtual void releaseImpl() {}
};

template<class ResourceType>
class ResourcePool final
{
public:
	using CreationFunction = std::function<std::unique_ptr<PooledType<ResourceType>>(ResourcePool& parentPool)>;
	ResourcePool(int initialPoolSize, const CreationFunction& creationFunction)
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
	ResourceType* getResource()
	{
		if (!mAvailable.empty())
		{
			PooledType<ResourceType>* resource = mAvailable.back();
			mAvailable.pop_back();
			return static_cast<ResourceType*>(resource);
		}

		return addNewResource();
	}

	void returnResource(PooledType<ResourceType>* resource)
	{
		mAvailable.push_back(resource);
	}

private:
	ResourceType* addNewResource()
	{
		mAllResources.push_back(mCreationFunction(*this));
		PooledType<ResourceType>* resource = mAllResources.back().get();
		mAvailable.push_back(resource);

		return static_cast<ResourceType*>(resource);
	}

	CreationFunction mCreationFunction;
	std::list<PooledType<ResourceType>*> mAvailable;
	std::list<std::unique_ptr<PooledType<ResourceType>>> mAllResources;
};
}
