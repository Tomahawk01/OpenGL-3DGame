#pragma once

#include <cmath>
#include <format>

namespace Game {

	struct vec4
	{
		constexpr vec4()
			: vec4(0.0f)
		{
		}

		constexpr vec4(float xyzw)
			: vec4(xyzw, xyzw, xyzw, xyzw)
		{
		}

		constexpr vec4(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w)
		{
		}

		constexpr bool operator==(const vec4&) const = default;

		float x;
		float y;
		float z;
		float w;
	};

	constexpr vec4& operator+=(vec4& v1, const vec4& v2)
	{
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
		v1.w += v2.w;

		return v1;
	}

	constexpr vec4 operator+(const vec4& v1, const vec4& v2)
	{
		vec4 temp = v1;
		return temp += v2;
	}

	constexpr vec4& operator-=(vec4& v1, const vec4& v2)
	{
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
		v1.w -= v2.w;

		return v1;
	}

	constexpr vec4 operator-(const vec4& v1, const vec4& v2)
	{
		vec4 temp = v1;
		return temp -= v2;
	}

}

template<>
struct std::formatter<Game::vec4>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::vec4& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "x={} y={} z={} w={}", obj.x, obj.y, obj.z, obj.w);
	}
};
