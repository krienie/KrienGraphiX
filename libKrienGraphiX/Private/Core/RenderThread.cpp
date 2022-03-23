
#include "RenderThread.h"

namespace kgx::core
{
RenderThread::RenderThread()
    : mRunning(true), mNumBusyThreads(0u)
{
    const unsigned int numThreads = std::thread::hardware_concurrency();
    mThreadPool.reserve(numThreads);
    for (unsigned int i = 0u; i < numThreads; ++i)
    {
        mThreadPool.emplace_back(&RenderThread::processRenderCommands, this);
    }
}

RenderThread::~RenderThread()
{
    {
        std::unique_lock<std::mutex> lock(mEnqueueMutex);
        mRunning = false;
        mCvCommand.notify_all();
    }

    for (auto &thread : mThreadPool)
    {
        thread.join();
    }
}

void RenderThread::enqueueRenderCommand(RenderCommand cmd)
{
    std::lock_guard<std::mutex> lock(mEnqueueMutex);
    mCommands.emplace_back(std::move(cmd));
    mCvCommand.notify_one();
}

void RenderThread::flush()
{
    //TODO(KL): This only makes sure the render commands are fired. Implement an actual GPU fence as well (As a command itself that blocks)

    std::unique_lock<std::mutex> lock(mEnqueueMutex);
    mCvFinished.wait(lock, [this](){ return mCommands.empty() && mNumBusyThreads == 0u; });
}

void RenderThread::processRenderCommands()
{
    do
    {
        // wait for commands to show up
        std::unique_lock<std::mutex> lock(mEnqueueMutex);
        mCvCommand.wait(lock, [this]() { return !mRunning || !mCommands.empty(); });
        if (!mCommands.empty())
        {
            ++mNumBusyThreads;

            auto cmd = mCommands.front();
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
