#pragma once

#include "Math/Vector3.h"

namespace Game {

	class ObjectTransformer
	{
	public:
		virtual ~ObjectTransformer() = default;

		virtual void Update() = 0;
		virtual vec3 Position() const = 0;
	};

}
