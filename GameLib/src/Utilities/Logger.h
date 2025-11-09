#pragma once

#include "Formatter.h"

#include "Game/config.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <source_location>
#include <string_view>

namespace Game::Logger {

	namespace Impl {

		inline static auto logFile = []
		{
			std::ofstream f{ "log.txt", std::ios::app };
			if (!f)
			{
				std::terminate();
			}

			return f;
		}();

	}

	enum class Level
	{
		TRACE,
		INFO,
		WARN,
		ERR	// NOTE: ERROR defined in wingdi.h :(
	};

	inline bool gForceLog = false;

	template<Level L, class... Args>
	struct Print
	{
		Print(std::format_string<Args...> msg, Args&&... args, std::source_location loc = std::source_location::current())
		{
			if (!(gForceLog || Config::LoggingEnabled()))
			{
				return;
			}

			char c = '?';
			if constexpr (L == Level::TRACE)
				c = 'T';
			else if constexpr (L == Level::INFO)
				c = 'I';
			else if constexpr (L == Level::WARN)
				c = 'W';
			else if constexpr (L == Level::ERR)
				c = 'E';

			const std::filesystem::path path{ loc.file_name() };

			const auto logLine = std::format("[{}] {}:{} {}", c, path.filename().string(), loc.line(), std::format(msg, std::forward<Args>(args)...));

			std::println("{}", logLine);

			if constexpr (Config::logToFile)
			{
				Impl::logFile << logLine << std::endl;
			}
		}
	};

	template<Level L = {}, class... Args >
	Print(std::format_string<Args...> msg, Args&&...) -> Print<L, Args...>;

	template<class... Args>
	using Trace = Print<Level::TRACE, Args...>;

	template<class... Args>
	using Info = Print<Level::INFO, Args...>;

	template<class... Args>
	using Warn = Print<Level::WARN, Args...>;

	template<class... Args>
	using Error = Print<Level::ERR, Args...>;

}
