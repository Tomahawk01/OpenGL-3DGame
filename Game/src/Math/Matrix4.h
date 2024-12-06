#pragma once

#include "Vector3.h"

#include <array>
#include <format>
#include <span>

namespace Game {

	class mat4
	{
	public:
		constexpr mat4()
			: m_Elements({
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			})
		{}

		constexpr mat4(const std::array<float, 16u>& elements)
			: m_Elements(elements)
		{}

		constexpr mat4(const vec3& translation)
			: m_Elements({
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				translation.x, translation.y, translation.z, 1.0f
			})
		{}

		static mat4 LookAt(const vec3& eye, const vec3& lookAt, const vec3& up);
		static mat4 Perspective(float fov, float width, float height, float nearPlane, float farPlane);

		constexpr std::span<const float> data() const { return m_Elements; }

		friend constexpr mat4& operator*=(mat4& m1, const mat4& m2);

	private:
		std::array<float, 16u> m_Elements;
	};

	constexpr mat4& operator*=(mat4& m1, const mat4& m2)
	{
		mat4 result{};

		for (auto i = 0u; i < 4u; i++)
		{
			for (auto j = 0u; j < 4u; j++)
			{
				result.m_Elements[i + j * 4] = 0.0f;
				for (auto k = 0u; k < 4u; k++)
				{
					result.m_Elements[i + j * 4] += m1.m_Elements[i + k * 4] * m2.m_Elements[k + j * 4];
				}
			}
		}

		m1 = result;
		return m1;
	}

	constexpr mat4 operator*(const mat4& m1, const mat4& m2)
	{
		mat4 temp{ m1 };
		return temp *= m2;
	}

	inline mat4 mat4::LookAt(const vec3& eye, const vec3& lookAt, const vec3& up)
	{
		const vec3 f = vec3::Normalize(lookAt - eye);
		const vec3 upNormalized = vec3::Normalize(up);

		const vec3 s = vec3::Normalize(vec3::Cross(f, upNormalized));
		const vec3 u = vec3::Normalize(vec3::Cross(s, f));

		mat4 m{};
		m.m_Elements = {
			{s.x, u.x, -f.x, 0.0f,
			 s.y, u.y, -f.y, 0.0f,
			 s.z, u.z, -f.z, 0.0f,
			 0.0f, 0.0f, 0.0f, 1.0f}
		};

		return m * mat4{ -eye };
	}

	inline mat4 mat4::Perspective(float fov, float width, float height, float nearPlane, float farPlane)
	{
		mat4 m;

		const float aspectRatio = width / height;
		const float temp = std::tan(fov / 2.0f);
		const float t = temp * nearPlane;
		const float b = -t;
		const float r = t * aspectRatio;
		const float l = b * aspectRatio;

		m.m_Elements = {
			{(2.0f * nearPlane) / (r - l), 0.0f, 0.0f, 0.0f,
			 0.0f, (2.0f * nearPlane) / (t - b), 0.0f, 0.0f,
			 (r + l) / (r - l), (t + b) / (t - b), -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f,
			 0.0f, 0.0f, -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane), 0.0f}
		};

		return m;
	}

}

template<>
struct std::formatter<Game::mat4>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::mat4& obj, std::format_context& ctx) const
	{
		const float* data = obj.data().data();
		return std::format_to(ctx.out(), "{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}",
			data[0], data[4], data[8], data[12], 
			data[1], data[5], data[9], data[13],
			data[2], data[6], data[10], data[14], 
			data[3], data[7], data[11], data[15]);
	}
};
