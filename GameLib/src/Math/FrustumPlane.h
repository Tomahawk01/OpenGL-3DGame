#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix3.h"

namespace Game {

	struct FrustumPlane
	{
		FrustumPlane() = default;
		FrustumPlane(float a, float b, float c, float d);

		vec3 normal;
		float distance;
	};

	std::string to_string(const FrustumPlane& obj);

}
