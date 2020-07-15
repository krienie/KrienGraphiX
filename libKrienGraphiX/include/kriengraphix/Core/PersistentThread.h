
#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "Core/CommandList.h"

namespace kgx
{
    class PersistentThread final
    {
    public:
        PersistentThread();
        ~PersistentThread();
        PersistentThread(PersistentThread&) = delete;
        PersistentThread& operator=(PersistentThread&) = delete;

        template<class T,
            class = typename std::enable_if<std::is_base_of<CommandBase, T>::value>::type,
            typename... Args>
        void enqueueCommand(Args... args)
        {
            std::lock_guard<std::mutex> lock(m_enqueueMutex);
            m_enqueueCommandList->addNewCommand<T>(args...);
        }

    private:
        void processCommands();

        std::atomic<bool> m_running;
        std::thread *m_thread;
        std::mutex m_enqueueMutex;
        std::mutex m_flushMutex;
        std::condition_variable m_flushCV;
        CommandList *m_enqueueCommandList;
        CommandList *m_flushCommandList;
    };
}
