
#include "Core/LinearAllocator.h"

namespace kgx
{
    LinearAllocator::LinearAllocator(void* memPtr, size_t size)
        : m_head(static_cast<uchar*>(memPtr)), m_tail(static_cast<uchar*>(memPtr) + size),
        m_ptr(m_head), m_size(size)
    {
    }

    void* LinearAllocator::allocate(size_t size, size_t alignment)
    {
        uchar* newPtr = m_ptr + alignedSize(size, alignment);
        if (newPtr > m_tail)
        {
            return nullptr;
        }

        uchar* oldPtr = m_ptr;
        m_ptr = newPtr;
        return oldPtr;
    }

    void LinearAllocator::reset()
    {
        m_ptr = m_head;
    }

    uchar* LinearAllocator::getPtr() const
    {
        return m_ptr;
    }

    uchar LinearAllocator::alignedSize(size_t size, size_t alignment)
    {
        return static_cast<uchar>((size + alignment - 1) & ~(alignment - 1));
    }
}