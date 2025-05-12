#pragma once

#include <stacktrace>
#include <stdexcept>

namespace Game {

	class Exception
	{
	public:
		Exception(std::string what, uint32_t skip = 1u);

		std::string Stacktrace() const;

		std::string to_string() const;
		std::string_view what() const;

	private:
		std::string m_What;
		std::stacktrace m_Trace;
	};

}
