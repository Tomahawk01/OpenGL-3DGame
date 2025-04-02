#pragma once

#include "Math/Color.h"
#include "Math/Vector3.h"

#include <format>

namespace Game {

	struct LineData
	{
		vec3 position;
		Color color;
	};

}

template<>
struct std::formatter<Game::LineData>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::LineData& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "p={} c={}", obj.position, obj.color);
	}
};
