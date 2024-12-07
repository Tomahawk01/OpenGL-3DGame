#pragma once

#include <cmath>
#include <format>

namespace Game {

	struct vec3
	{
		static vec3 Normalize(const vec3& v)
		{
			const float length = std::hypot(v.x, v.y, v.z);
			if (length == 0.0f)
				return { 0.0f, 0.0f, 0.0f };

			return { .x = v.x / length, .y = v.y / length, .z = v.z / length };
		}

		static constexpr vec3 Cross(const vec3& v1, const vec3& v2)
		{
			const float i = (v1.y * v2.z) - (v1.z * v2.y);
			const float j = (v1.x * v2.z) - (v1.z * v2.x);
			const float k = (v1.x * v2.y) - (v1.y * v2.x);

			return { i, -j, k };
		}

		constexpr bool operator==(const vec3&) const = default;

		float x;
		float y;
		float z;
	};

	constexpr vec3& operator+=(vec3& v1, const vec3& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;

		return v1;
	}

	constexpr vec3 operator+(const vec3& v1, const vec3& v2)
	{
		vec3 temp = v1;
		return temp += v2;
	}

	constexpr vec3& operator-=(vec3& v1, const vec3& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;

		return v1;
	}

	constexpr vec3 operator-(const vec3& v1, const vec3& v2)
	{
		vec3 temp = v1;
		return temp -= v2;
	}

	constexpr vec3 operator-(const vec3& v)
	{
		return { -v.x, -v.y, -v.z };
	}

}

template<>
struct std::formatter<Game::vec3>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::vec3& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "x={} y={} z={}", obj.x, obj.y, obj.z);
	}
};
