#pragma once

#include "Formatter.h"
#include "AutoRelease.h"
#include "Exception.h"
#include "Logger.h"

#include <memory>
#include <stacktrace>

namespace Game {

	template<class ...Args>
	constexpr void Expect(bool predicate, std::format_string<Args...> msg, Args&&... args)
	{
		if (!predicate)
		{
			Logger::Error("{}", std::format(msg, std::forward<Args>(args)...));
			Logger::Error("{}", std::stacktrace::current(1));
			std::terminate();
			std::unreachable();
		}
	}

	template<class T, class ...Args>
	constexpr void Expect(std::unique_ptr<T>& obj, std::format_string<Args...> msg, Args&&... args)
	{
		Expect(!!obj, msg, std::forward<Args>(args)...);
	}

	template<class ...Args>
	void Ensure(bool predicate, std::format_string<Args...> msg, Args&&... args)
	{
		if (!predicate)
			throw Exception(msg, std::forward<Args>(args)...);
	}

	template<class T, T Invalid, class ...Args>
	void Ensure(AutoRelease<T, Invalid>& obj, std::format_string<Args...> msg, Args&&... args)
	{
		Ensure(!!obj, msg, std::forward<Args>(args)...);
	}

	template<class T, class D, class ...Args>
	void Ensure(std::unique_ptr<T, D>& obj, std::format_string<Args...> msg, Args&&... args)
	{
		Ensure(!!obj, msg, std::forward<Args>(args)...);
	}

}
