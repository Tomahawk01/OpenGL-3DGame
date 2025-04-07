#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix3.h"

#include <format>

namespace Game {

	struct FrustumPlane
	{
		FrustumPlane() = default;
		FrustumPlane(float a, float b, float c, float d);

		vec3 normal;
		float distance;
	};

}

template<>
struct std::formatter<Game::FrustumPlane>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::FrustumPlane& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "normal=[{}] distance=[{}]", obj.normal, obj.distance);
	}
};
