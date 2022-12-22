
#include "CommandThread.h"

namespace kgx::core
{
CommandThread::CommandThread(unsigned int numWorkerThreads)
    : mRunning(true), mNumBusyThreads(0u)
{
    mWorkerThreads.reserve(numWorkerThreads);
    for (unsigned int i = 0u; i < numWorkerThreads; ++i)
    {
        mWorkerThreads.emplace_back(&CommandThread::processThreadCommands, this);
    }
}

CommandThread::~CommandThread()
{
    {
        std::unique_lock lock(mEnqueueMutex);
        mRunning = false;
        mCvCommand.notify_all();
    }

    for (auto &thread : mWorkerThreads)
    {
        thread.join();
    }
}

void CommandThread::enqueueCommand(ThreadCommand cmd)
{
    std::lock_guard lock(mEnqueueMutex);
    mCommands.emplace_back(std::move(cmd));
    mCvCommand.notify_one();
}

void CommandThread::flush()
{
    std::unique_lock lock(mEnqueueMutex);
    mCvFinished.wait(lock, [this](){ return mCommands.empty() && mNumBusyThreads == 0u; });
}

void CommandThread::processThreadCommands()
{
    do
    {
        // wait for commands to show up
        std::unique_lock lock(mEnqueueMutex);
        mCvCommand.wait(lock, [this]() { return !mRunning || !mCommands.empty(); });
        if (mRunning && !mCommands.empty())
        {
            ++mNumBusyThreads;

            ThreadCommand cmd = mCommands.front();
            mCommands.pop_front();

            // Run the command
            lock.unlock();
            cmd();
            lock.lock();

            --mNumBusyThreads;
            mCvFinished.notify_one();
        }
    } while (mRunning);
}

}
