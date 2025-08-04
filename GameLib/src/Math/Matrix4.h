#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Utilities/Error.h"

#include <array>
#include <ranges>
#include <format>
#include <span>

namespace Game {

	class mat4
	{
	public:
		struct Scale {};

		constexpr mat4()
			: m_Elements({
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			})
		{}

		explicit mat4(const std::array<float, 16u>& elements)
			: mat4{ std::span<const float>{elements} }
		{}

		explicit mat4(const std::span<const float>& elements)
			: mat4{}
		{
			Ensure(elements.size() == 16u, "Not enough elements");
			std::ranges::copy(elements, std::ranges::begin(m_Elements));
		}

		explicit constexpr mat4(const vec3& translation)
			: m_Elements({
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				translation.x, translation.y, translation.z, 1.0f
			})
		{}

		constexpr mat4(const vec3& scale, Scale)
			: m_Elements({
				scale.x, 0.0f, 0.0f, 0.0f,
				0.0f, scale.y, 0.0f, 0.0f,
				0.0f, 0.0f, scale.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			})
		{}

		constexpr mat4(const vec3& translation, const vec3& scale)
			: m_Elements({
				scale.x, 0.0f, 0.0f, 0.0f,
				0.0f, scale.y, 0.0f, 0.0f,
				0.0f, 0.0f, scale.z, 0.0f,
				translation.x, translation.y, translation.z, 1.0f
				})
		{}

		constexpr mat4(const quat& rotation)
			: mat4{}
		{
			m_Elements[0] = 1.0f - 2.0f * rotation.Y * rotation.Y - 2.0f * rotation.Z * rotation.Z;
			m_Elements[1] = 2.0f * rotation.X * rotation.Y + 2.0f * rotation.Z * rotation.W;
			m_Elements[2] = 2.0f * rotation.X * rotation.Z - 2.0f * rotation.Y * rotation.W;

			m_Elements[4] = 2.0f * rotation.X * rotation.Y - 2.0f * rotation.Z * rotation.W;
			m_Elements[5] = 1.0f - 2.0f * rotation.X * rotation.X - 2.0f * rotation.Z * rotation.Z;
			m_Elements[6] = 2.0f * rotation.Y * rotation.Z + 2.0f * rotation.X * rotation.W;

			m_Elements[8] = 2.0f * rotation.X * rotation.Z + 2.0f * rotation.Y * rotation.W;
			m_Elements[9] = 2.0f * rotation.Y * rotation.Z - 2.0f * rotation.X * rotation.W;
			m_Elements[10] = 1.0f - 2.0f * rotation.X * rotation.X - 2.0f * rotation.Y * rotation.Y;
		}

		static mat4 LookAt(const vec3& eye, const vec3& lookAt, const vec3& up);
		static mat4 Perspective(float fov, float width, float height, float nearPlane, float farPlane);
		static mat4 Orthographic(float width, float height, float depth);

		constexpr std::span<const float> data() const { return m_Elements; }

		constexpr auto& operator[](this auto&& self, size_t index) { return self.m_Elements[index]; }

		constexpr vec4 Row(size_t index) const
		{
			Expect(index <= 3, "Index out of range");

			return { m_Elements[index], m_Elements[index + 4u], m_Elements[index + 8u], m_Elements[index + 12u] };
		}

		friend constexpr mat4& operator*=(mat4& m1, const mat4& m2);

		constexpr bool operator==(const mat4&) const = default;

		std::string to_string() const;

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
				auto sum = 0.0f;
				for (auto k = 0u; k < 4u; k++)
				{
					sum += m1.m_Elements[i + k * 4] * m2.m_Elements[k + j * 4];
				}
				result.m_Elements[i + j * 4] = sum;
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

	constexpr vec3 operator*(const mat4& m, const vec3& v)
	{
		const auto row1 = m.Row(0);
		const auto row2 = m.Row(1);
		const auto row3 = m.Row(2);

		return {
			v.x * row1.x + v.y * row1.y + v.z * row1.z + row1.w,
			v.x * row2.x + v.y * row2.y + v.z * row2.z + row2.w,
			v.x * row3.x + v.y * row3.y + v.z * row3.z + row3.w
		};
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

	inline mat4 mat4::Orthographic(float width, float height, float depth)
	{
		const float right = width / 2.0f;
		const float left = -right;
		const float top = height / 2.0f;
		const float bottom = -top;
		const float far_ = depth;
		const float near_ = 0.0f;

		mat4 m{};
		m.m_Elements = {{
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (far_ - near_), 0.0f,
			-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far_ + near_) / (far_ - near_), 1.0f
		}};

		return m;
	}

	inline std::string mat4::to_string() const
	{
		const float* d = data().data();
		return std::format("{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}",
			d[0], d[4], d[8], d[12], 
			d[1], d[5], d[9], d[13],
			d[2], d[6], d[10], d[14], 
			d[3], d[7], d[11], d[15]);
	}

}
