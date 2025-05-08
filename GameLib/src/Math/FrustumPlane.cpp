#include "FrustumPlane.h"

#include <format>

namespace Game {

	FrustumPlane::FrustumPlane(float a, float b, float c, float d)
	{
		const float invLength = 1.0f / std::hypot(a, b, c);
		normal = vec3(a * invLength, b * invLength, c * invLength);
		distance = d * invLength;
	}

	std::string to_string(const FrustumPlane& obj)
	{
		return std::format("normal=[{}] distance=[{}]", obj.normal, obj.distance);
	}

}
