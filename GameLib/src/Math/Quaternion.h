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

		std::string to_string() const;

		float X;
		float Y;
		float Z;
		float W;
	};

	inline std::string quat::to_string() const
	{
		return std::format("x={} y={} z={} w={}", X, Y, Z, W);
	}

}
