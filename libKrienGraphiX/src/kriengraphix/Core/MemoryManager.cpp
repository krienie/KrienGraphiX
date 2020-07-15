
#include "Core/MemoryManager.h"

namespace kgx
{
    MemoryManager::~MemoryManager()
    {
        for (auto memPtr : m_dataBlobs)
        {
            delete memPtr;
        }
    }

    MemoryManager::MemoryManager(MemoryManager&& other) noexcept
        : m_dataBlobs(std::move(other.m_dataBlobs))
    {
    }

    MemoryManager& MemoryManager::operator=(MemoryManager&& rhs) noexcept
    {
        m_dataBlobs = std::move(rhs.m_dataBlobs);
        return *this;
    }

    void* MemoryManager::allocate(size_t size)
    {
        uchar* newData = static_cast<uchar*>(::operator new(size));
        m_dataBlobs.insert(newData);
        return newData;
    }

    void MemoryManager::deallocate(uchar* dataPtr)
    {
        auto it = m_dataBlobs.find(dataPtr);
        if (it != m_dataBlobs.end())
        {
            delete *it;
            m_dataBlobs.erase(it);
        }
    }
}