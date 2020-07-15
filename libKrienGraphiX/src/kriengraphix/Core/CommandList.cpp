
#include "Core/CommandList.h"

namespace
{
    constexpr size_t default_mem_size = 64u * 1024u;

    size_t getNextPowerOfTwo(size_t x)
    {
        if (x <= 1)
        {
            return 1;
        }

        size_t power = 2;
        x--;
        while (x >>= 1)
        {
            power <<= 1;
        }

        return power;
    }
}

namespace kgx
{
    CommandList::CommandList()
        : m_linAllocators(), m_curLinAllocator(), m_memManager(),
        m_head(nullptr), m_tail(nullptr)
    {
        initDefaultAllocators();
    }

    CommandList::~CommandList()
    {
        // Destruct all remaining commands
        if (m_head)
        {
            CommandBase* curCmd = m_head;
            do
            {
                CommandBase* next = curCmd->next;
                curCmd->~CommandBase();
                curCmd = next;
            } while (curCmd != nullptr);
        }

        // Deallocate all used memory
        for (auto &it : m_linAllocators)
        {
            m_memManager.deallocate(it.getPtr());
        }
    }

    /*CommandList::CommandList(CommandList&& other) noexcept
        : m_linAllocators(std::move(other.m_linAllocators)), m_curLinAllocator(other.m_curLinAllocator),
            m_memManager(std::move(other.m_memManager)), m_head(std::exchange(other.m_head, nullptr)), m_tail(std::exchange(other.m_tail, nullptr))
    {
        // Make sure other is left in a valid state.
        other.initDefaultAllocators();
    }

    CommandList& CommandList::operator=(CommandList&& rhs) noexcept
    {
        m_curLinAllocator = rhs.m_curLinAllocator;
        m_linAllocators   = std::move(rhs.m_linAllocators);
        m_memManager      = std::move(rhs.m_memManager);
        m_head            = std::exchange(rhs.m_head, nullptr);
        m_tail            = std::exchange(rhs.m_tail, nullptr);

        // Make sure rhs is left in a valid state.
        rhs.initDefaultAllocators();

        return *this;
    }*/

    bool CommandList::isEmpty() const
    {
        return m_head == nullptr;
    }

    void CommandList::flushCommands()
    {
        if (!m_head)
        {
            return;
        }

        CommandBase* curCmd = m_head;
        do
        {
            CommandBase* next = curCmd->next;
            curCmd->execute();
            curCmd->~CommandBase();
            curCmd = next;
        } while (curCmd != nullptr);

        // Reset all linear allocators
        for (auto &linAlloc : m_linAllocators)
        {
            linAlloc.reset();
        }

        m_curLinAllocator = m_linAllocators.begin();
        m_head = m_tail = nullptr;
    }

    void CommandList::initDefaultAllocators()
    {
        void* defMem = m_memManager.allocate(default_mem_size);
        m_linAllocators.emplace_back(defMem, default_mem_size);
        m_curLinAllocator = m_linAllocators.begin();
    }

    void* CommandList::getNextAllocationPoint(size_t size, size_t alignment)
    {
        // Try to find an empty spot in the current allocator pool
        void* allocPoint = m_curLinAllocator->allocate(size, alignment);
        while (!allocPoint && m_curLinAllocator != --m_linAllocators.end())
        {
            ++m_curLinAllocator;
            allocPoint = m_curLinAllocator->allocate(size, alignment);
        }

        if (!allocPoint)
        {
            // No free space left, time to allocate new memory
            size_t newMemSize = size > default_mem_size ? getNextPowerOfTwo(size) : default_mem_size;
            void* newMemPtr = m_memManager.allocate(newMemSize);

            m_linAllocators.emplace_back(newMemPtr, newMemSize);
            ++m_curLinAllocator; // get iterator to the last element

            allocPoint = m_curLinAllocator->allocate(size, alignment);
        }

        return allocPoint;
    }
}
