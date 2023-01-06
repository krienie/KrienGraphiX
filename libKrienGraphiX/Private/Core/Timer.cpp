
#include "Timer.h"

#include <algorithm>
#include <cassert>
#include <cstdint>

namespace kgx::core
{
Timer::Timer(int intervalMilliSeconds, OnCompletedEvent onCompleted)
    : mIntervalMilliSeconds(std::clamp(intervalMilliSeconds, 0, INT32_MAX)),
        mOnCompletedEvent(std::move(onCompleted))
{
    assert(mOnCompletedEvent != nullptr);
    mWorkerThread = std::make_unique<std::thread>(&Timer::timerTick, this);
}

Timer::~Timer()
{
    stop();
}

void Timer::stop()
{
    mIsRunning = false;

    if (mWorkerThread)
    {
        mWorkerThread->join();
        mWorkerThread.reset();
    }
}

void Timer::timerTick()
{
    auto getCurrentMillisecondTime = []()
    {
        using namespace std::chrono;
        
        const auto epoch = system_clock::now().time_since_epoch();
        return std::chrono::duration_cast<milliseconds>(epoch).count();
    };

    mIsRunning = true;
    
    do
    {
        const auto now = getCurrentMillisecondTime();

        if (mPrevEvent + mIntervalMilliSeconds < now)
        {
            const float deltaTimeSeconds = static_cast<float>(now - mPrevEvent) / 1000.0f;
            mOnCompletedEvent(deltaTimeSeconds);
            mPrevEvent = now;
        }
    }
    while (mIsRunning);
}
}
