#pragma once

#include "Vector3.h"

namespace Game {

	struct UV
	{
		float x;
		float y;
	};

	struct VertexData
	{
		vec3 position;
		vec3 normal;
		UV uv;
	};

}
