
#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include <vector>
#include <functional>

namespace kgx::core
{

class CommandThread final
{
    public:
        using ThreadCommand = std::function<void()>;

        CommandThread(unsigned int numWorkerThreads = std::thread::hardware_concurrency());
        ~CommandThread();

        CommandThread(const CommandThread&) noexcept            = delete;
        CommandThread(CommandThread&&) noexcept                 = delete;
        CommandThread& operator=(const CommandThread&) noexcept = delete;
        CommandThread& operator=(CommandThread&&) noexcept      = delete;

        void enqueueCommand(ThreadCommand cmd);
        void flush();

    private:
        void processThreadCommands();

        bool mRunning;
        std::vector<std::thread> mWorkerThreads;
        std::mutex mEnqueueMutex;

        std::deque<ThreadCommand> mCommands;
        std::condition_variable mCvCommand;
        std::condition_variable mCvFinished;
        unsigned int mNumBusyThreads;
};
}
