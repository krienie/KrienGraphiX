
#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include <vector>
#include <functional>

namespace kgx::core
{

class RenderThread final
{
    public:
        using RenderCommand = std::function<void()>;

        RenderThread();
        ~RenderThread();

        RenderThread(const RenderThread&) noexcept            = delete;
        RenderThread(RenderThread&&) noexcept                 = delete;
        RenderThread& operator=(const RenderThread&) noexcept = delete;
        RenderThread& operator=(RenderThread&&) noexcept      = delete;

        void enqueueRenderCommand(RenderCommand cmd);
        void flush();

    private:
        void processRenderCommands();

        bool mRunning;
        std::vector<std::thread> mThreadList;
        std::mutex mEnqueueMutex;

        std::deque<std::function<void()>> mCommands;
        std::condition_variable mCvCommand;
        std::condition_variable mCvFinished;
        unsigned int mNumBusyThreads;
};
}
