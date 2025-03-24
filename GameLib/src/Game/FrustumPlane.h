#pragma once

#include "Math/Vector3.h"

namespace Game {

	struct FrustumPlane
	{
		vec3 normal;
		float distance;
	};

}
