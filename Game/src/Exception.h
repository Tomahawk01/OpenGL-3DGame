#pragma once

#include <stdexcept>
#include <stacktrace>

namespace Game {

	class Exception : public std::runtime_error
	{
	public:
		Exception(const std::string& what);

		std::string Stacktrace() const;

	private:
		std::stacktrace m_Trace;
	};

}
