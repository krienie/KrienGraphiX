#pragma once

#include <unordered_set>

namespace kgx
{
    // TODO: wellicht in een losse header gooien...
    typedef unsigned char uchar;

    class MemoryManager final
    {
        public:
            MemoryManager() = default;
            ~MemoryManager();
            MemoryManager(MemoryManager& other) = delete;
            MemoryManager(MemoryManager&& other) noexcept;
            MemoryManager& operator=(MemoryManager& rhs) = delete;
            MemoryManager& operator=(MemoryManager&& rhs) noexcept;

            void* allocate(size_t size);
            void deallocate(uchar* dataPtr);

        private:
            std::unordered_set<uchar*> m_dataBlobs;
    };
}
