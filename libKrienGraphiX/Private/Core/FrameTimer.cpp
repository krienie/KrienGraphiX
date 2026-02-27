
#include "FrameTimer.h"

#include <SDL3/SDL_timer.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>

namespace kgx::core
{
FrameTimer::FrameTimer(int targetFPS, OnFrameTick onFrameTick)
	: mTargetFPS(std::max(1, targetFPS)),
		mOnFrameTickEvent(std::move(onFrameTick))
{
	mIsRunning = true;
	assert(mOnFrameTickEvent != nullptr);
	mWorkerThread = std::make_unique<std::thread>(&FrameTimer::timerTick, this);
}

FrameTimer::~FrameTimer()
{
	stop();
}

void FrameTimer::stop()
{
	mIsRunning = false;

	if (mWorkerThread)
	{
		mWorkerThread->join();
		mWorkerThread.reset();
	}
}

void FrameTimer::timerTick() const
{
	constexpr uint64_t nsPerSecond = 1000000000;
	const uint64_t nsPerFrame = nsPerSecond / static_cast<uint64_t>(mTargetFPS);

	uint64_t prevFrameTime = SDL_GetTicksNS();
	uint64_t nextFrameTime = prevFrameTime + nsPerFrame;

	do
	{
		const uint64_t currentFrameTime = SDL_GetTicksNS();
		const float deltaTime = static_cast<float>(currentFrameTime - prevFrameTime) / static_cast<float>(nsPerFrame);
		prevFrameTime = currentFrameTime;

		mOnFrameTickEvent(deltaTime);

		const uint64_t now = SDL_GetTicksNS();
		if (now < nextFrameTime)
		{

			SDL_DelayPrecise(nextFrameTime - now);
		}

		nextFrameTime += nsPerFrame;
	}
	while (mIsRunning);
}
}
