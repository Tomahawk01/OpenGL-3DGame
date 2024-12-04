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

template<>
struct std::formatter<Game::Exception>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::Exception& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}\n{}", obj.what(), obj.Stacktrace());
	}
};
