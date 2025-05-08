#pragma once

#include "Utilities/Formatter.h"
#include "Math/Color.h"
#include "Math/Vector3.h"

namespace Game {

	struct LineData
	{
		vec3 position;
		Color color;
	};

	inline std::string to_string(const LineData& obj)
	{
		return std::format("p=[{}] c=[{}]", obj.position, obj.color);
	}

}
