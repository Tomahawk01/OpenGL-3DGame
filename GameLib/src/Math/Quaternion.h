#pragma once

#include <format>

namespace Game {

	class quat
	{
	public:
		constexpr quat()
			: quat(0.0f, 0.0f, 0.0f, 1.0f)
		{}

		constexpr quat(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w)
		{}

		float X;
		float Y;
		float Z;
		float W;
	};

}

template<>
struct std::formatter<Game::quat>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::quat& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "x={} y={} z={} w={}", obj.X, obj.Y, obj.Z, obj.W);
	}
};
