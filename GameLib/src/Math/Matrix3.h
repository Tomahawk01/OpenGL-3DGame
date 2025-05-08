#pragma once

#include "Utilities/Error.h"
#include "Vector3.h"

#include <array>
#include <span>
#include <ranges>
#include <format>

namespace Game {

	class mat3
	{
	public:
		constexpr mat3()
			: m_Elements({
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f
				})
		{
		}

		constexpr mat3(const std::array<float, 9u>& elements)
			: m_Elements(elements)
		{
		}

		constexpr mat3(const vec3& v1, const vec3& v2, const vec3& v3)
			: m_Elements({
				v1.x, v1.y, v1.z,
				v2.x, v2.y, v2.z,
				v3.x, v3.y, v3.z,
				})
		{
		}

		constexpr std::span<const float> data() const { return m_Elements; }

		constexpr auto& operator[](this auto&& self, size_t index) { return self.m_Elements[index]; }

		static constexpr mat3 Invert(const mat3& m)
		{
			const mat3 adjoint{ {
				(m[4] * m[8]) - (m[5] * m[7]),
				-((m[1] * m[8]) - (m[2] * m[7])),
				(m[1] * m[5]) - (m[2] * m[4]),
				-((m[3] * m[8]) - (m[5] * m[6])),
				(m[0] * m[8]) - (m[2] * m[6]),
				-((m[0] * m[5]) - (m[2] * m[3])),
				(m[3] * m[7]) - (m[4] * m[6]),
				-((m[0] * m[7]) - (m[1] * m[6])),
				(m[0] * m[4]) - (m[1] * m[3])
			} };

			const float determinant{
				(m[0] * m[4] * m[8]) +
				(m[3] * m[7] * m[2]) +
				(m[6] * m[1] * m[5]) -
				(m[0] * m[7] * m[5]) -
				(m[3] * m[1] * m[8]) -
				(m[6] * m[4] * m[2])
			};

			const auto invertedValues = adjoint.data() |
				std::views::transform([determinant](auto e) { return e / determinant; });

			std::array<float, 9u> invertedArray{};
			std::ranges::copy(invertedValues, std::ranges::begin(invertedArray));

			return { invertedArray };
		}

		static constexpr mat3 Transpose(mat3 m)
		{
			std::ranges::swap(m[1], m[3]);
			std::ranges::swap(m[2], m[6]);
			std::ranges::swap(m[5], m[7]);

			return m;
		}

		vec3 Row(size_t index) const
		{
			Ensure(index <= 2, "Index out of range");

			return { m_Elements[index], m_Elements[index + 3u], m_Elements[index + 6u] };
		}

		friend constexpr mat3& operator*=(mat3& m1, const mat3& m2);

		constexpr bool operator==(const mat3&) const = default;

		std::string to_string() const;

	private:
		std::array<float, 9u> m_Elements;
	};

	constexpr mat3& operator*=(mat3& m1, const mat3& m2)
	{
		mat3 result{};

		for (auto i = 0u; i < 3u; i++)
		{
			for (auto j = 0u; j < 3u; j++)
			{
				result.m_Elements[i + j * 3] = 0.0f;
				for (auto k = 0u; k < 3u; k++)
				{
					result.m_Elements[i + j * 3] += m1.m_Elements[i + k * 3] * m2.m_Elements[k + j * 3];
				}
			}
		}

		m1 = result;
		return m1;
	}

	constexpr mat3 operator*(const mat3& m1, const mat3& m2)
	{
		mat3 temp{ m1 };
		return temp *= m2;
	}

	constexpr vec3 operator*(const mat3& m, const vec3& v)
	{
		return {
			m[0] * v.x + m[3] * v.y + m[6] * v.z,
			m[1] * v.x + m[4] * v.y + m[7] * v.z,
			m[2] * v.x + m[5] * v.y + m[8] * v.z
		};
	}

	inline std::string mat3::to_string() const
	{
		const float* d = data().data();
		return std::format("{} {} {}\n{} {} {}\n{} {} {}",
			d[0], d[3], d[6],
			d[1], d[4], d[7],
			d[2], d[5], d[8]);
	}

}
