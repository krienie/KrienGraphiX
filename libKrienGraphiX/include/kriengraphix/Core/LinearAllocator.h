#pragma once

namespace kgx
{
    typedef unsigned char uchar;

    class LinearAllocator
    {
    public:
        LinearAllocator(void* memPtr, size_t size);

        void* allocate(size_t size, size_t alignment);
        void reset();
        uchar* getPtr() const;

    private:
        uchar alignedSize(size_t size, size_t alignment);

        uchar* m_head;
        uchar* m_tail;
        uchar* m_ptr;
        size_t m_size;
    };
}
