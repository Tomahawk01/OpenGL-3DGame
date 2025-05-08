#pragma once

#include <stacktrace>
#include <stdexcept>

namespace Game {

	class Exception : public std::runtime_error
	{
	public:
		Exception(const std::string& what, uint32_t skip = 1u);

		std::string Stacktrace() const;

		std::string to_string() const;

	private:
		std::stacktrace m_Trace;
	};

}
