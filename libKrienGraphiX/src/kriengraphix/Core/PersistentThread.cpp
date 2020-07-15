
#include <condition_variable>
#include <thread>

#include "Core/PersistentThread.h"

namespace kgx
{
    PersistentThread::PersistentThread()
        : m_running(true), m_thread(nullptr), m_enqueueMutex(), m_flushMutex(), m_flushCV(),
            m_enqueueCommandList(new CommandList()), m_flushCommandList(new CommandList())
    {
        m_thread = new std::thread(&PersistentThread::processCommands, this);
    }

    PersistentThread::~PersistentThread()
    {
        m_running = false;
        m_thread->join();

        delete m_enqueueCommandList;
        delete m_flushCommandList;
        delete m_thread;
    }

    void PersistentThread::processCommands()
    {
        do
        {
            // wait for commands to show up
            while (m_enqueueCommandList->isEmpty() && m_running)
            {
                std::this_thread::yield();
            }
            
            {
                std::lock_guard<std::mutex> enqueueLock(m_enqueueMutex);
                std::swap(m_flushCommandList, m_enqueueCommandList);
            }
            
            m_flushCommandList->flushCommands();
        } while (m_running);
    }
}
