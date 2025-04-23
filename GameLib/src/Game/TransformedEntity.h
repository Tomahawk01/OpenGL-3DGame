#pragma once

#include "Math/Vector3.h"
#include "Math/AABB.h"
#include "Renderer/Camera.h"
#include "Core/Entity.h"

#include "Chain.h"

#include <memory>

namespace Game {

	struct GameTransformState
	{
		const Camera& camera;
		AABB aabb;
		vec3 lastCameraPos;
	};

	struct TransformedEntity
	{
		Entity entity;
		AABB boundingBox;
		std::unique_ptr<ChainBase<GameTransformState>> transformerChain;
	};

}
