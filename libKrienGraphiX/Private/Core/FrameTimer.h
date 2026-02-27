
#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace kgx::core
{
class FrameTimer final
{
public:
	using OnFrameTick = std::function<void(float deltaTime)>;

	FrameTimer(int targetFPS, OnFrameTick onFrameTick);
	~FrameTimer();

	void stop();

	FrameTimer(const FrameTimer&) noexcept            = delete;
	FrameTimer& operator=(const FrameTimer&) noexcept = delete;
	FrameTimer(FrameTimer&&) noexcept                 = delete;
	FrameTimer& operator=(FrameTimer&&) noexcept      = delete;

private:
	void timerTick() const;

	std::unique_ptr<std::thread> mWorkerThread;

	std::atomic<bool> mIsRunning = false;
	int mTargetFPS;

	OnFrameTick mOnFrameTickEvent;
};
}
