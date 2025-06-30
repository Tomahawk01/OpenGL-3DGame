#include "Exception.h"

#include <format>

namespace Game {

	std::string Exception::Stacktrace() const
	{
		return std::to_string(m_Trace);
	}

	std::string Exception::to_string() const
	{
		return std::format("{}\n{}", what(), Stacktrace());
	}

	std::string_view Exception::what() const
	{
		return m_What;
	}

}
