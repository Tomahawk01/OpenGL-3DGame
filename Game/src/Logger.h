#pragma once

#include <format>
#include <print>
#include <source_location>
#include <string_view>

namespace Game::Logger {

	enum class Level
	{
		TRACE,
		INFO,
		WARN,
		ERROR
	};

	template<Level L, class... Args>
	struct Print {};

	template<Level L, class... Args>
	struct Print<L, const char*, Args...>
	{
		Print(const char* msg, Args&&... args, std::source_location loc = std::source_location::current())
		{
			char c = '?';
			if constexpr (L == Level::TRACE)
				c = 'T';
			else if constexpr (L == Level::INFO)
				c = 'I';
			else if constexpr (L == Level::WARN)
				c = 'W';
			else if constexpr (L == Level::ERROR)
				c = 'E';

			std::println("[{}] {}:{} {}", c, loc.file_name(), loc.line(), std::vformat(msg, std::make_format_args(args...)));
		}
	};

	template<Level L, class... Args>
	Print(Args...) -> Print<L, Args...>;

	template<class... Args>
	using Trace = Print<Level::TRACE, Args...>;

	template<class... Args>
	using Info = Print<Level::INFO, Args...>;

	template<class... Args>
	using Warn = Print<Level::WARN, Args...>;

	template<class... Args>
	using Error = Print<Level::ERROR, Args...>;

}
