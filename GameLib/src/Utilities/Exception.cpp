#include "Exception.h"

#include <format>

namespace Game {

	Exception::Exception(const std::string& what, uint32_t skip)
		: std::runtime_error(what), m_Trace(std::stacktrace::current(skip))
	{}

	std::string Exception::Stacktrace() const
	{
		return std::to_string(m_Trace);
	}

	std::string Exception::to_string() const
	{
		return std::format("{}\n{}", what(), Stacktrace());
	}

}
