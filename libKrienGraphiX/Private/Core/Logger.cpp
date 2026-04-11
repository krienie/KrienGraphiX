
#include "Logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "KrienGraphiX/Core/Logging.h"

namespace kgx::core
{
Logger::Logger()
{
	spdlog::init_thread_pool(8192, 1);

	constexpr bool rotateOnOpen = true;
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto rotatingSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
		"logs/kgxlog.log",
		1024 * 1024 * 5,
		3,
		rotateOnOpen
	);

	std::vector<spdlog::sink_ptr> sinks {consoleSink, rotatingSink};

	auto logger = std::make_shared<spdlog::async_logger>(
		"KrienGraphiX",
		sinks.begin(),
		sinks.end(),
		spdlog::thread_pool(),
		spdlog::async_overflow_policy::block
	);

	spdlog::register_logger(logger);
	spdlog::set_default_logger(logger);

#ifdef DEBUG
	spdlog::set_level(spdlog::level::debug);
#else
	spdlog::set_level(spdlog::level::info);
#endif

	spdlog::flush_every(std::chrono::seconds(3));
	spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");

	KGXLOG_DEBUG("Logging initialized");
}

Logger::~Logger()
{
	spdlog::shutdown();
}
}
