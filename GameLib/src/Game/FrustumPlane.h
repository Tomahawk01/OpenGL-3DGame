#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix3.h"

#include <format>

namespace Game {

	struct FrustumPlane
	{
		vec3 normal;
		float distance;
	};

	inline vec3 Intersection(const FrustumPlane& p1, const FrustumPlane& p2, const FrustumPlane& p3)
	{
		const mat3 a{ p1.normal, p2.normal, p3.normal };
		const vec3 b{ -p1.distance, -p2.distance, -p3.distance };

		return mat3::Invert(a) * b;
	}

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
