
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace kgx::core
{
class Timer final
{
public:
    using OnCompletedEvent = std::function<void(float deltaTime)>;

    Timer(int intervalMilliseconds, OnCompletedEvent onCompleted);
    ~Timer();

    void stop();

    Timer(const Timer&) noexcept            = delete;
    Timer& operator=(const Timer&) noexcept = delete;
    Timer(Timer&&) noexcept                 = delete;
    Timer& operator=(Timer&&) noexcept      = delete;

private:
    void timerTick();

    std::unique_ptr<std::thread> mWorkerThread;

    std::atomic<bool> mIsRunning = false;
    int mIntervalMilliSeconds;
    int64_t mPrevEvent = 0;

    OnCompletedEvent mOnCompletedEvent;
};
}
