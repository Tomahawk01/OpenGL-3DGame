#include "FrustumPlane.h"

namespace Game {

	FrustumPlane::FrustumPlane(float a, float b, float c, float d)
	{
		const float invLength = 1.0f / std::hypot(a, b, c);
		normal = vec3(a * invLength, b * invLength, c * invLength);
		distance = d * invLength;
	}

}
