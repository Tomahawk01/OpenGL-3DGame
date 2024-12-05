#include "Exception.h"

namespace Game {

	Exception::Exception(const std::string& what, std::uint32_t skip)
		: std::runtime_error(what), m_Trace(std::stacktrace::current(skip))
	{}

	std::string Exception::Stacktrace() const
	{
		return std::to_string(m_Trace);
	}

}
