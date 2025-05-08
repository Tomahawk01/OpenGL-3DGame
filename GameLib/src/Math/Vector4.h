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

		static constexpr float Dot(const vec4& v1, const vec4& v2)
		{
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
		}

		constexpr bool operator==(const vec4&) const = default;

		std::string to_string() const;

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

	inline std::string vec4::to_string() const
	{
		return std::format("x={} y={} z={} w={}", x, y, z, w);
	}

}
