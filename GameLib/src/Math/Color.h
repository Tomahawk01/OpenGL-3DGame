#pragma once

#include <format>

namespace Game {

	struct Color
	{
		float r;
		float g;
		float b;
	};

	inline std::string to_string(const Color& obj)
	{
		return std::format("r={} g={} b={}", obj.r, obj.g, obj.b);
	}

};
