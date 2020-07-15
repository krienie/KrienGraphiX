#pragma once

#include <type_traits>
#include <forward_list>

#include "Core/Commands/CommandBase.h"
#include "Core/LinearAllocator.h"
#include "Core/MemoryManager.h"

namespace kgx
{
    class CommandList final
    {
        public:
            CommandList();
            ~CommandList();
            CommandList(CommandList&) = delete;
            CommandList(CommandList&& other) noexcept = delete;
            CommandList& operator=(CommandList&) = delete;
            CommandList& operator=(CommandList&& rhs) noexcept = delete;

            [[nodiscard]]
            bool isEmpty() const;

            template<class T,
                     class = typename std::enable_if<std::is_base_of<CommandBase, T>::value>::type,
                     typename... Args>
            void addNewCommand(Args... args)
            {
                void* dataPtr = getNextAllocationPoint(sizeof(T), alignof(T));
                CommandBase* newCmd = new (dataPtr) T(args...);
                if (m_head)
                {
                    m_tail->next = newCmd;
                    m_tail = newCmd;
                } else
                {
                    m_head = m_tail = newCmd;
                }
            }

            void flushCommands();

        private:
            void initDefaultAllocators();
            void* getNextAllocationPoint(size_t size, size_t alignment);

            std::list<LinearAllocator> m_linAllocators;
            std::list<LinearAllocator>::iterator m_curLinAllocator;
            MemoryManager m_memManager;
            CommandBase* m_head;
            CommandBase* m_tail;
    };
}

