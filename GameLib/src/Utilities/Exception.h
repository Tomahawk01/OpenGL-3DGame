#pragma once

#include <stacktrace>
#include <stdexcept>
#include <format>

namespace Game {

	class Exception
	{
	public:
		template<class ...Args>
		Exception(std::format_string<Args...> msg, Args&&... args)
			: m_What(std::format(msg, std::forward<Args>(args)...))
			, m_Trace(std::stacktrace::current(1))
		{}

		std::string Stacktrace() const;

		std::string to_string() const;
		std::string_view what() const;

	private:
		std::string m_What;
		std::stacktrace m_Trace;
	};

}
