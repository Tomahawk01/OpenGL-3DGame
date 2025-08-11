#pragma once

#include <format>

namespace Game {

	struct Color
	{
		float r;
		float g;
		float b;
	};

	namespace Colors {
		inline constexpr Color White{ 1.0f, 1.0f, 1.0f };
		inline constexpr Color Azure{ 0.0f, 0.5f, 1.0f };
	}

	inline std::string to_string(const Color& obj)
	{
		return std::format("r={} g={} b={}", obj.r, obj.g, obj.b);
	}

};
